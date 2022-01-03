#include "buildcc.h"

#include "build.foo.h"

using namespace buildcc;

static void clean_cb();
static void foolib_build_cb(BaseTarget &target);

constexpr std::string_view EXE = "build";

int main(int argc, char **argv) {
  // 1. Get arguments
  Args args;
  ArgToolchain arg_gcc;
  ArgToolchain arg_msvc;
  args.AddToolchain("gcc", "Generic gcc toolchain", arg_gcc);
  args.AddToolchain("msvc", "Generic msvc toolchain", arg_msvc);
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

  // 5.
  reg.RunBuild();

  // 6.
  plugin::ClangCompileCommands({&g_foolib, &m_foolib}).Generate();

  return 0;
}

static void clean_cb() {
  env::log_info(EXE, fmt::format("Cleaning {}", env::get_project_build_dir()));
  fs::remove_all(env::get_project_build_dir());
}

static void foolib_build_cb(BaseTarget &target) {
  fooTarget(target, "");
  target.AddSource("main.cpp");
  target.Build();
}
