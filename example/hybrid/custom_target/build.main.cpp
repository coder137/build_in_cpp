#include "buildcc.h"

using namespace buildcc;

static void clean_cb();
static void foolib_build_cb(BaseTarget &target);

static constexpr std::string_view EXE = "build";

int main(int argc, char **argv) {
  // Get arguments
  ArgToolchain arg_gcc;
  ArgToolchain arg_msvc;
  ArgToolchain arg_toolchain_clang_gnu;
  ArgTarget arg_target_clang_gnu;
  Args::Init()
      .AddToolchain("gcc", "Generic gcc toolchain", arg_gcc)
      .AddToolchain("msvc", "Generic msvc toolchain", arg_msvc)
      .AddToolchain("clang_gnu", "Clang GNU toolchain", arg_toolchain_clang_gnu)
      .AddTarget("clang_gnu", "Clang GNU target", arg_target_clang_gnu)
      .Parse(argc, argv);

  // Initialize your environment
  Reg::Init();

  // Pre-build steps
  Reg::Call(Args::Clean()).Func(clean_cb);

  // Build steps
  Toolchain_gcc gcc;
  ExecutableTarget_gcc g_foolib("foolib", gcc, "");
  Reg::Toolchain(arg_gcc.state).Build(foolib_build_cb, g_foolib);

  Toolchain_msvc msvc;
  ExecutableTarget_msvc m_foolib("foolib", msvc, "");
  Reg::Toolchain(arg_msvc.state).Build(foolib_build_cb, m_foolib);

  // * NOTE, This is how we add our custom toolchain
  BaseToolchain clang = arg_toolchain_clang_gnu.ConstructToolchain();
  Target_custom c_foolib("CFoolib.exe", TargetType::Executable, clang, "",
                         arg_target_clang_gnu.GetTargetConfig());
  Reg::Toolchain(arg_toolchain_clang_gnu.state)
      .Build(foolib_build_cb, c_foolib);

  //
  Reg::Run();

  //
  plugin::ClangCompileCommands({&g_foolib, &m_foolib, &c_foolib}).Generate();

  return 0;
}

static void clean_cb() {
  env::log_info(EXE, fmt::format("Cleaning {}", Project::GetBuildDir()));
  fs::remove_all(Project::GetBuildDir());
}

static void foolib_build_cb(BaseTarget &target) {
  target.AddSource("src/foo.cpp");
  target.AddIncludeDir("src", true);
  target.AddSource("main.cpp");
  target.Build();
}
