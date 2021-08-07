#include "buildcc.h"

#include "build.foo.h"

using namespace buildcc;

static void clean_cb();
static void foolib_build_cb(base::Target &target);

int main(int argc, char **argv) {
  // 1. Get arguments
  Args args;
  args.Parse(argc, argv);

  // 2. Initialize your environment
  Register reg(args);
  reg.Env();

  // 3. Pre-build steps
  reg.Clean(clean_cb);

  // 4. Build steps
  Toolchain_gcc gcc;
  Toolchain_msvc msvc;

  ExecutableTarget_gcc g_foolib("GFoolib", gcc, "");
  ExecutableTarget_msvc m_foolib("MFoolib", msvc, "");

  reg.Build(args.GetGccState(), g_foolib, foolib_build_cb);
  reg.Build(args.GetMsvcState(), m_foolib, foolib_build_cb);

  // 5.
  reg.RunBuild();

  // 6.
  plugin::ClangCompileCommands({&g_foolib, &m_foolib}).Generate();

  return 0;
}

static void clean_cb() {
  // TODO,
}

static void foolib_build_cb(base::Target &target) {
  fooTarget(target, "");
  target.AddSource("main.cpp");
  target.Build();
}
