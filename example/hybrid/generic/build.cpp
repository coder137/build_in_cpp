#include "buildcc.h"

#include "clang_compile_commands.h"

#include "flatbuffers/util.h"
#include "fmt/format.h"

// Libraries
#include "build.foo.h"

using namespace buildcc;

constexpr std::string_view EXE = "build";

// Function Prototypes
static void clean_cb();
static void generic_build_cb(base::Target &generic_cppflags);

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

  Args args;
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
      ->transform(CLI::CheckedTransformer(toolchain_id_map_, CLI::ignore_case));
  user_toolchain->add_option("--name", toolchain_name);
  user_toolchain->add_option("--asm_compiler", toolchain_asm_compiler);
  user_toolchain->add_option("--c_compiler", toolchain_c_compiler);
  user_toolchain->add_option("--cpp_compiler", toolchain_cpp_compiler);
  user_toolchain->add_option("--archiver", toolchain_archiver);
  user_toolchain->add_option("--linker", toolchain_linker);

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

  // Target specific settings
  Target_generic generic_target("GenericTarget.exe",
                                base::TargetType::Executable, toolchain, "src");
  reg.Build(custom_toolchain, generic_target, generic_build_cb);

  // 5. Test steps
  reg.Test(custom_toolchain, generic_target, [](base::Target &target) {
    internal::Command::Execute(
        fmt::format("{}", target.GetTargetPath().string()));
  });

  // 6. Build Target
  reg.RunBuild();

  // 7. Test Target
  reg.RunTest();

  // 8. Post Build steps

  // - Clang Compile Commands
  plugin::ClangCompileCommands({&generic_target}).Generate();

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

static void generic_build_cb(base::Target &generic_cppflags) {
  fooTarget(generic_cppflags, env::get_project_root_dir() / ".." / "foolib");
  generic_cppflags.AddSource("main.cpp");
  generic_cppflags.Build();
}
