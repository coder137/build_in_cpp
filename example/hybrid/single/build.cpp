#include "buildcc.h"

using namespace buildcc;

constexpr const char *const EXE = "build";

// Function Prototypes
static void clean_cb();
static void hello_world_build_cb(BaseTarget &target);

int main(int argc, char **argv) {
  // 1. Get arguments
  Args::Init();
  ArgToolchain arg_gcc;
  Args::AddToolchain("gcc", "Generic gcc toolchain", arg_gcc);
  Args::Parse(argc, argv);

  // 2. Initialize your environment
  Register reg;

  // 3. Pre-build steps
  reg.Clean(clean_cb);

  // 4. Build steps
  // Explicit toolchain - target pairs
  Toolchain_gcc gcc;
  gcc.Verify();

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
  env::log_info(EXE, fmt::format("Cleaning {}", Project::GetBuildDir()));
  fs::remove_all(Project::GetBuildDir());
}

static void hello_world_build_cb(BaseTarget &target) {
  target.AddSource("main.cpp", "src");

  target.Build();
}
