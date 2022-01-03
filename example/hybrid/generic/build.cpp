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
static void foolib_build_cb(BaseTarget &foolib_target);
static void generic_build_cb(BaseTarget &generic_target,
                             BaseTarget &foolib_target);

int main(int argc, char **argv) {
  // 1. Get arguments
  ArgToolchain custom_toolchain;
  TargetType default_lib_type{TargetType::StaticLibrary};
  Args args;

  try {
    const std::map<std::string, TargetType> lib_type_map_{
        {"StaticLib", TargetType::StaticLibrary},
        {"DynamicLib", TargetType::DynamicLibrary},
    };

    args.Ref()
        .add_option("--default_lib_type", default_lib_type, "Default Lib Type")
        ->transform(CLI::CheckedTransformer(lib_type_map_, CLI::ignore_case))
        ->group("Custom");

    // NOTE, You can add more custom toolchains as per your requirement
    args.AddToolchain("user", "User defined toolchain", custom_toolchain);
  } catch (const std::exception &e) {
    std::cout << "EXCEPTION " << e.what() << std::endl;
  }

  args.Parse(argc, argv);

  // 2. Initialize your environment
  Register reg(args);

  // 3. Pre-build steps
  reg.Clean(clean_cb);

  // 4. Build steps
  // Toolchain + Generic Target
  BaseToolchain toolchain = custom_toolchain.ConstructToolchain();
  Target_generic foolib_target("libfoo", default_lib_type, toolchain, "");
  reg.Build(custom_toolchain.state, foolib_build_cb, foolib_target);

  // Target specific settings
  Target_generic generic_target("generic", TargetType::Executable, toolchain,
                                "src");
  reg.Build(custom_toolchain.state, generic_build_cb, generic_target,
            foolib_target);
  reg.Dep(generic_target, foolib_target);

  // 5. Test steps
  reg.Test(custom_toolchain.state, "{executable}", generic_target);

  // 6. Build Target
  reg.RunBuild();

  // 7. Post Build steps
  // For Static Lib do nothing
  // For Dynamic Lib we need to handle special cases
  // - MSVC behaviour
  // - Copy to executable location
  if (default_lib_type == TargetType::DynamicLibrary) {

    // MSVC special case
    fs::path copy_from_path;
    fs::path copy_to_path;
    if (toolchain.GetId() == ToolchainId::Msvc) {
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
    if (generic_target.IsBuilt()) {
      fs::remove(copy_to_path);
      fs::copy(copy_from_path, copy_to_path);
    }
  }

  // 8. Test Target
  reg.RunTest();

  // - Clang Compile Commands
  plugin::ClangCompileCommands({&foolib_target, &generic_target}).Generate();

  // - Plugin Graph
  std::string output = reg.GetTaskflow().dump();
  const bool saved = env::save_file("graph.dot", output, false);
  env::assert_fatal(saved, "Could not save graph.dot file");

  return 0;
}

static void clean_cb() {
  env::log_info(EXE, fmt::format("Cleaning {}", env::get_project_build_dir()));
  fs::remove_all(env::get_project_build_dir());
}

static void foolib_build_cb(BaseTarget &foolib_target) {
  fooTarget(foolib_target, env::get_project_root_dir() / ".." / "foolib");
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
