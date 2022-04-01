#include <algorithm>

// Core build lib
#include "buildcc.h"

// Third party-libs
#include "fmt/format.h"

// Libraries
#include "build.foo.h"

using namespace buildcc;

constexpr std::string_view EXE = "build";

// Function Prototypes
static void clean_cb();
static void args_lib_type_cb(CLI::App &app, TargetType &lib_type);
static void foolib_build_cb(BaseTarget &foolib_target);
static void generic_build_cb(BaseTarget &generic_target,
                             BaseTarget &foolib_target);

static void post_build_cb(BaseTarget &generic_target,
                          BaseTarget &foolib_target);

int main(int argc, char **argv) {
  // Get arguments
  ArgToolchain custom_toolchain;
  TargetType default_lib_type{TargetType::StaticLibrary};
  Args::Init()
      .AddToolchain("user", "User defined toolchain", custom_toolchain)
      .AddCustomCallback(
          [&](CLI::App &app) { args_lib_type_cb(app, default_lib_type); })
      .Parse(argc, argv);

  // Initialize your environment
  Reg::Init();

  // Pre-build steps
  Reg::Call(Args::Clean()).Func(clean_cb);

  // Build steps
  // Toolchain + Generic Target
  BaseToolchain toolchain = custom_toolchain.ConstructToolchain();
  Target_generic foolib_target("libfoo", default_lib_type, toolchain, "");
  Target_generic generic_target("generic", TargetType::Executable, toolchain,
                                "src");
  Reg::Toolchain(custom_toolchain.state)
      .Func([&]() { toolchain.Verify(); })
      .Build(foolib_build_cb, foolib_target)
      .Build(generic_build_cb, generic_target, foolib_target)
      .Dep(generic_target, foolib_target)
      .Test("{executable}", generic_target);

  // Build Target
  RegConfig reg_config;
  reg_config.post_build_cb = [&]() {
    post_build_cb(generic_target, foolib_target);
  };
  Reg::Run(reg_config);

  // - Clang Compile Commands
  plugin::ClangCompileCommands({&foolib_target, &generic_target}).Generate();

  // - Plugin Graph
  std::string output = Reg::GetTaskflow().dump();
  const bool saved = env::save_file("graph.dot", output, false);
  env::assert_fatal(saved, "Could not save graph.dot file");

  return 0;
}

static void clean_cb() {
  env::log_info(EXE, fmt::format("Cleaning {}", Project::GetBuildDir()));
  fs::remove_all(Project::GetBuildDir());
}

void args_lib_type_cb(CLI::App &app, TargetType &lib_type) {
  const std::map<std::string, TargetType> lib_type_map_{
      {"StaticLib", TargetType::StaticLibrary},
      {"DynamicLib", TargetType::DynamicLibrary},
  };

  app.add_option("--default_lib_type", lib_type, "Default Lib Type")
      ->transform(CLI::CheckedTransformer(lib_type_map_, CLI::ignore_case))
      ->group("Custom");
}

static void foolib_build_cb(BaseTarget &foolib_target) {
  fooTarget(foolib_target, Project::GetRootDir() / ".." / "foolib");
  foolib_target.Build();
}

static void generic_build_cb(BaseTarget &generic_target,
                             BaseTarget &foolib_target) {
  const auto &foolib_include_dirs = foolib_target.GetIncludeDirs();
  std::for_each(
      foolib_include_dirs.cbegin(), foolib_include_dirs.cend(),
      [&](const fs::path &p) { generic_target.AddIncludeDir(p, true); });
  generic_target.AddLibDep(foolib_target);
  generic_target.AddSource("main.cpp");
  generic_target.Build();
}

void post_build_cb(BaseTarget &generic_target, BaseTarget &foolib_target) {
  // For Static Lib do nothing
  // For Dynamic Lib we need to handle special cases
  // - MSVC behaviour
  // - Copy to executable location
  if (foolib_target.GetType() == TargetType::DynamicLibrary) {
    // MSVC special case
    fs::path copy_from_path;
    fs::path copy_to_path;
    if (foolib_target.GetToolchain().GetId() == ToolchainId::Msvc) {
      copy_from_path =
          fmt::format("{}.dll", path_as_string(foolib_target.GetTargetPath()));
      copy_to_path =
          generic_target.GetTargetBuildDir() /
          fmt::format("{}.dll",
                      foolib_target.GetTargetPath().filename().string());
    } else {
      copy_from_path = foolib_target.GetTargetPath();
      copy_to_path =
          generic_target.GetTargetBuildDir() /
          (foolib_target.GetName() + foolib_target.GetConfig().target_ext);
    }

    // Copy case
    // TODO, This should be baked into the `Target` API
    if (generic_target.IsBuilt()) {
      fs::remove(copy_to_path);
      fs::copy(copy_from_path, copy_to_path);
    }
  }
}
