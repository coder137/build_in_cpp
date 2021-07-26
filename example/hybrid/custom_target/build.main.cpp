#include "buildcc.h"

using namespace buildcc;

static void clean_cb();
static void foolib_build_cb(base::Target &target);

static constexpr std::string_view EXE = "build";

int main(int argc, char **argv) {
  // 1. Get arguments
  Args args;
  Args::ToolchainArg toolchain_clang_gnu;
  Args::TargetArg target_clang_gnu;
  args.AddCustomToolchain("clang_gnu", "Clang GNU toolchain",
                          toolchain_clang_gnu);
  args.AddCustomTarget("clang_gnu", "Clang GNU target", target_clang_gnu);
  args.Parse(argc, argv);

  // 2. Initialize your environment
  Register reg(args);
  reg.Env();

  // 3. Pre-build steps
  reg.Clean(clean_cb);

  // 4. Build steps
  Toolchain_gcc gcc;
  Toolchain_msvc msvc;

  ExecutableTarget_gcc g_foolib("GFoolib.exe", gcc, "");
  ExecutableTarget_msvc m_foolib("MFoolib.exe", msvc, "");
  m_foolib.AddCppCompileFlag("/nologo");
  m_foolib.AddCppCompileFlag("/EHsc");
  m_foolib.AddLinkFlag("/nologo");

  reg.Build(args.GetGccState(), g_foolib, foolib_build_cb);
  reg.Build(args.GetMsvcState(), m_foolib, foolib_build_cb);

  // * NOTE, This is how we add our custom toolchain
  base::Toolchain clang = toolchain_clang_gnu.ConstructToolchain();

  // * M1, Hardcode it
  // constexpr std::string_view clang_compile_command =
  //     "{compiler} {preprocessor_flags} {include_dirs} {compile_flags} -o "
  //     "{output} -c {input}";
  // constexpr std::string_view clang_link =
  //     "{cpp_compiler} {link_flags} {compiled_sources} -o {output} "
  //     "{lib_dirs} {lib_deps}";

  // * M2, Get from Args (see build_linux.toml or build_win.toml files)
  Target_custom c_foolib("CFoolib.exe", base::TargetType::Executable, clang, "",
                         target_clang_gnu.compile_command,
                         target_clang_gnu.link_command);
  reg.Build(toolchain_clang_gnu.state, c_foolib, foolib_build_cb);

  // 5.
  reg.RunBuild();

  // 6.
  plugin::ClangCompileCommands({&g_foolib, &m_foolib, &c_foolib}).Generate();

  return 0;
}

static void clean_cb() {
  env::log_info(
      EXE, fmt::format("Cleaning {}", env::get_project_build_dir().string()));
  fs::remove_all(env::get_project_build_dir());
}

static void foolib_build_cb(base::Target &target) {
  target.AddSource("src/foo.cpp");
  target.AddIncludeDir("src", true);
  target.AddSource("main.cpp");
  target.Build();
}
