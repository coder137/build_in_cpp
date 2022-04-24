#include "buildcc.h"

#include "build.foo.h"

using namespace buildcc;

static void clean_cb();
static void foolib_build_cb(BaseTarget &target, const TargetInfo &foolib);

constexpr const char *const EXE = "build";

int main(int argc, char **argv) {
  // Get arguments
  ArgToolchain arg_gcc;
  ArgToolchain arg_msvc;
  Args::Init()
      .AddToolchain("gcc", "Generic gcc toolchain", arg_gcc)
      .AddToolchain("msvc", "Generic msvc toolchain", arg_msvc)
      .Parse(argc, argv);

  // Initialize your environment
  Reg::Init();

  // Pre-build steps
  Reg::Call(Args::Clean()).Func(clean_cb);

  // Build steps
  Toolchain_gcc gcc;
  TargetInfo g_foo(gcc, "../foolib");
  ExecutableTarget_gcc g_external("cppflags", gcc, "");
  Reg::Toolchain(arg_gcc.state)
      .Func(fooTarget, g_foo)
      .Build(foolib_build_cb, g_external, g_foo);

  Toolchain_msvc msvc;
  ExecutableTarget_msvc m_external("cppflags", msvc, "");
  TargetInfo m_foo(gcc, "../foolib");
  Reg::Toolchain(arg_msvc.state)
      .Func(fooTarget, m_foo)
      .Build(foolib_build_cb, m_external, m_foo);

  //
  Reg::Run();

  //
  plugin::ClangCompileCommands({&g_external, &m_external}).Generate();

  return 0;
}

static void clean_cb() {
  env::log_info(EXE, fmt::format("Cleaning {}", Project::GetBuildDir()));
  fs::remove_all(Project::GetBuildDir());
}

static void foolib_build_cb(BaseTarget &target, const TargetInfo &foolib) {
  target.AddSource("main.cpp");
  target.Insert(foolib, {
                            SyncOption::SourceFiles,
                            SyncOption::HeaderFiles,
                            SyncOption::IncludeDirs,
                        });
  target.Build();
}
