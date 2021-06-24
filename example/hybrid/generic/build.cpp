#include <algorithm>

// Core build lib
#include "buildcc.h"

// Third party-libs
#include "flatbuffers/util.h"
#include "fmt/format.h"

// Libraries
#include "build.foo.h"

using namespace buildcc;

constexpr std::string_view EXE = "build";

// Function Prototypes
static void clean_cb();
static void foolib_build_cb(base::Target &foolib_target);
static void generic_build_cb(base::Target &generic_target,
                             base::Target &foolib_target);

int main(int argc, char **argv) {
  // 1. Get arguments
  base::Toolchain::Id toolchain_id;
  std::string toolchain_name;
  std::string toolchain_asm_compiler;
  std::string toolchain_c_compiler;
  std::string toolchain_cpp_compiler;
  std::string toolchain_archiver;
  std::string toolchain_linker;
  Args::Toolchain custom_toolchain;

  base::TargetType default_lib_type{base::TargetType::StaticLibrary};
  Args args;

  try {
    const std::map<std::string, base::TargetType> lib_type_map_{
        {"StaticLib", base::TargetType::StaticLibrary},
        {"DynamicLib", base::TargetType::DynamicLibrary},
    };

    args.Ref()
        .add_option("--default_lib_type", default_lib_type, "Default Lib Type")
        ->transform(CLI::CheckedTransformer(lib_type_map_, CLI::ignore_case))
        ->group("Custom");

    args.AddCustomToolchain("user", "User defined toolchain", custom_toolchain);
    auto *user_toolchain = args.Ref()
                               .get_subcommand("toolchain")
                               ->get_subcommand("user")
                               ->group("Custom");

    // clang-format off
    const std::map<std::string, base::Toolchain::Id> toolchain_id_map_{
        {"Gcc", base::Toolchain::Id::Gcc},
        {"Msvc", base::Toolchain::Id::Msvc},
        // {"Clang", base::Toolchain::Id::Clang}, // Currently not supported
    };
    // clang-format on
    user_toolchain->add_option("--id", toolchain_id, "Toolchain ID settings")
        ->transform(
            CLI::CheckedTransformer(toolchain_id_map_, CLI::ignore_case));
    user_toolchain->add_option("--name", toolchain_name);
    user_toolchain->add_option("--asm_compiler", toolchain_asm_compiler);
    user_toolchain->add_option("--c_compiler", toolchain_c_compiler);
    user_toolchain->add_option("--cpp_compiler", toolchain_cpp_compiler);
    user_toolchain->add_option("--archiver", toolchain_archiver);
    user_toolchain->add_option("--linker", toolchain_linker);
  } catch (const std::exception &e) {
    std::cout << "EXCEPTION " << e.what() << std::endl;
  }

  args.Parse(argc, argv);

  // 2. Initialize your environment
  Register reg(args);
  reg.Env();

  // 3. Pre-build steps
  reg.Clean(clean_cb);

  // 4. Build steps
  // Toolchain + Generic Target
  base::Toolchain toolchain(toolchain_id, toolchain_name,
                            toolchain_asm_compiler, toolchain_c_compiler,
                            toolchain_cpp_compiler, toolchain_archiver,
                            toolchain_linker);

  Target_generic foolib_target("libfoo", default_lib_type, toolchain, "");
  reg.Build(custom_toolchain, foolib_target, foolib_build_cb);

  // Target specific settings
  Target_generic generic_target("generic", base::TargetType::Executable,
                                toolchain, "src");
  auto g_cb = std::bind(generic_build_cb, std::placeholders::_1,
                        std::ref(foolib_target));
  reg.Build(custom_toolchain, generic_target, g_cb);
  reg.Dep(generic_target, foolib_target);

  // 5. Test steps
  reg.Test(custom_toolchain, generic_target, [](base::Target &target) {
    const bool execute = internal::Command::Execute(
        fmt::format("{}", target.GetTargetPath().string()));
    env::assert_fatal(execute, "Test failed");
  });

  // 6. Build Target
  reg.RunBuild();

  // 7. Post Build steps
  // For Static Lib do nothing
  // For Dynamic Lib we need to handle special cases
  // - MSVC behaviour
  // - Copy to executable location
  if (default_lib_type == base::TargetType::DynamicLibrary) {

    // MSVC special case
    fs::path copy_from_path;
    fs::path copy_to_path;
    if (toolchain.GetId() == base::Toolchain::Id::Msvc) {
      copy_from_path = foolib_target.GetTargetPath().string() + ".dll";
      copy_to_path = generic_target.GetTargetIntermediateDir() /
                     (foolib_target.GetName() + ".dll");
    } else {
      copy_from_path = foolib_target.GetTargetPath();
      copy_to_path =
          generic_target.GetTargetIntermediateDir() / foolib_target.GetName();
    }

    // Copy case
    if (generic_target.FirstBuild() || generic_target.Rebuild()) {
      env::log_info(EXE, "COPY");
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
  const bool saved = flatbuffers::SaveFile("graph.dot", output, false);
  env::assert_fatal(saved, "Could not save graph.dot file");

  return 0;
}

static void clean_cb() {
  env::log_info(
      EXE, fmt::format("Cleaning {}", env::get_project_build_dir().string()));
  fs::remove_all(env::get_project_build_dir());
}

static void foolib_build_cb(base::Target &foolib_target) {
  fooTarget(foolib_target, env::get_project_root_dir() / ".." / "foolib");
  foolib_target.Build();
}

static void generic_build_cb(base::Target &generic_target,
                             base::Target &foolib_target) {
  const auto &foolib_include_dirs = foolib_target.GetCurrentIncludeDirs();
  std::for_each(
      foolib_include_dirs.cbegin(), foolib_include_dirs.cend(),
      [&](const fs::path &p) { generic_target.AddIncludeDir(p, true); });
  generic_target.AddLibDep(foolib_target);
  generic_target.AddSource("main.cpp");
  generic_target.Build();
}
