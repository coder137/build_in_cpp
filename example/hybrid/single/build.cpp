#include "buildcc.h"

using namespace buildcc;

constexpr const char *const EXE = "build";

// Function Prototypes
static void clean_cb();
static void hello_world_build_cb(BaseTarget &target);

int main(int argc, char **argv) {
  // 1. Get arguments
  Args args;
  ArgToolchain arg_gcc;
  args.AddToolchain("gcc", "Generic gcc toolchain", arg_gcc);
  args.Parse(argc, argv);

  // 2. Initialize your environment
  Register reg(args);

  // 3. Pre-build steps
  reg.Clean(clean_cb);

  // 4. Build steps
  // Explicit toolchain - target pairs
  Toolchain_gcc gcc;
  auto verified_toolchains = gcc.Verify();
  env::assert_fatal(!verified_toolchains.empty(), "GCC Toolchain not found");

  ExecutableTarget_gcc hello_world("hello_world", gcc, "");

  // Select your builds and tests using the .toml files
  reg.Build(arg_gcc.state, hello_world_build_cb, hello_world);

  // 5. Test steps
  reg.Test(arg_gcc.state, "{executable}", hello_world);

  // 6. Build Target
  reg.RunBuild();

  // 7. Test Target
  reg.RunTest();

  // 8. Post Build steps
  // - Clang Compile Commands
  plugin::ClangCompileCommands({&hello_world}).Generate();

  return 0;
}

static void clean_cb() {
  env::log_info(EXE, fmt::format("Cleaning {}", env::get_project_build_dir()));
  fs::remove_all(env::get_project_build_dir());
}

static void hello_world_build_cb(BaseTarget &target) {
  target.AddSource("main.cpp", "src");

  target.Build();
}
