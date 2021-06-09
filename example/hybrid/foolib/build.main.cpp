#include "build.foo.h"

#include "clang_compile_commands.h"

using namespace buildcc;

static void clean_cb();
static void gfoolib_build_cb(base::Target &target);
static void mfoolib_build_cb(base::Target &target);

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

  ExecutableTarget_gcc g_foolib("GCppFlags.exe", gcc, "");
  ExecutableTarget_msvc m_foolib("MCppFlags.exe", msvc, "");

  reg.Build(args.GetGccToolchain(), g_foolib, gfoolib_build_cb);
  reg.Build(args.GetMsvcToolchain(), m_foolib, mfoolib_build_cb);

  // 5.
  reg.RunBuild();

  // 6.
  plugin::ClangCompileCommands({&g_foolib, &m_foolib}).Generate();

  return 0;
}

static void clean_cb() {
  // TODO,
}

static void gfoolib_build_cb(base::Target &target) {
  fooTarget(target, "");
  target.AddSource("main.cpp");
  target.Build();
}

static void mfoolib_build_cb(base::Target &target) {
  target.AddCppCompileFlag("/nologo");
  target.AddCppCompileFlag("/EHsc");
  target.AddLinkFlag("/nologo");
  fooTarget(target, "");
  target.AddSource("main.cpp");
  target.Build();
}
