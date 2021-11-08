#include "buildcc.h"

using namespace buildcc;

static void clean_cb();
static void foolib_build_cb(base::Target &target);

static constexpr std::string_view EXE = "build";

int main(int argc, char **argv) {
  // 1. Get arguments
  Args args;
  Args::ToolchainArg arg_gcc;
  Args::ToolchainArg arg_msvc;
  Args::ToolchainArg toolchain_clang_gnu;
  Args::TargetArg target_clang_gnu;
  args.AddToolchain("gcc", "Generic gcc toolchain", arg_gcc);
  args.AddToolchain("msvc", "Generic msvc toolchain", arg_msvc);
  args.AddToolchain("clang_gnu", "Clang GNU toolchain", toolchain_clang_gnu);
  args.AddTarget("clang_gnu", "Clang GNU target", target_clang_gnu);
  args.Parse(argc, argv);

  // 2. Initialize your environment
  Register reg(args);

  // 3. Pre-build steps
  reg.Clean(clean_cb);

  // 4. Build steps
  Toolchain_gcc gcc;
  Toolchain_msvc msvc;

  ExecutableTarget_gcc g_foolib("foolib", gcc, "");
  ExecutableTarget_msvc m_foolib("foolib", msvc, "");

  reg.Build(arg_gcc.state, foolib_build_cb, g_foolib);
  reg.Build(arg_msvc.state, foolib_build_cb, m_foolib);

  // * NOTE, This is how we add our custom toolchain
  base::Toolchain clang = toolchain_clang_gnu.ConstructToolchain();

  // * M2, Get from Args (see build_linux.toml or build_win.toml files)
  base::Target::Config config;
  config.compile_command = target_clang_gnu.compile_command;
  config.link_command = target_clang_gnu.link_command;
  Target_custom c_foolib("CFoolib.exe", base::Target::Type::Executable, clang,
                         "", config);
  reg.Build(toolchain_clang_gnu.state, foolib_build_cb, c_foolib);

  // 5.
  reg.RunBuild();

  // 6.
  plugin::ClangCompileCommands({&g_foolib, &m_foolib, &c_foolib}).Generate();

  return 0;
}

static void clean_cb() {
  env::log_info(EXE, fmt::format("Cleaning {}", env::get_project_build_dir()));
  fs::remove_all(env::get_project_build_dir());
}

static void foolib_build_cb(base::Target &target) {
  target.AddSource("src/foo.cpp");
  target.AddIncludeDir("src", true);
  target.AddSource("main.cpp");
  target.Build();
}
