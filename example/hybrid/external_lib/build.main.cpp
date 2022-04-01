#include "buildcc.h"

#include "build.foo.h"

using namespace buildcc;

static void clean_cb();
static void foolib_build_cb(BaseTarget &target);

constexpr const char *const EXE = "build";

int main(int argc, char **argv) {
  // 1. Get arguments
  ArgToolchain arg_gcc;
  ArgToolchain arg_msvc;
  Args::Init()
      .AddToolchain("gcc", "Generic gcc toolchain", arg_gcc)
      .AddToolchain("msvc", "Generic msvc toolchain", arg_msvc)
      .Parse(argc, argv);

  // 2. Initialize your environment
  Reg::Init();

  // 3. Pre-build steps
  Reg::Call(Args::Clean()).Func(clean_cb);

  // 4. Build steps
  Toolchain_gcc gcc;
  ExecutableTarget_gcc g_foolib("cppflags", gcc, "");
  Reg::Toolchain(arg_gcc.state).Build(foolib_build_cb, g_foolib);

  Toolchain_msvc msvc;
  ExecutableTarget_msvc m_foolib("cppflags", msvc, "");
  Reg::Toolchain(arg_msvc.state).Build(foolib_build_cb, m_foolib);

  // 5.
  Reg::Run();

  // 6.
  plugin::ClangCompileCommands({&g_foolib, &m_foolib}).Generate();

  return 0;
}

static void clean_cb() {
  env::log_info(EXE, fmt::format("Cleaning {}", Project::GetBuildDir()));
  fs::remove_all(Project::GetBuildDir());
}

static void foolib_build_cb(BaseTarget &target) {
  fooTarget(target, "../foolib");
  target.AddSource("main.cpp");
  target.Build();
}
