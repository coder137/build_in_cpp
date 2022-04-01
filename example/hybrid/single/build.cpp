#include "buildcc.h"

using namespace buildcc;

constexpr const char *const EXE = "build";

// Function Prototypes
static void clean_cb();
static void hello_world_build_cb(BaseTarget &target);

int main(int argc, char **argv) {
  // 1. Get arguments
  ArgToolchain arg_gcc;
  Args::Init()
      .AddToolchain("gcc", "Generic gcc toolchain", arg_gcc)
      .Parse(argc, argv);

  // 2. Initialize your environment
  Reg::Init();

  // 3. Pre-build steps
  Reg::Call(Args::Clean()).Func(clean_cb);

  // 4. Build steps
  // Explicit toolchain - target pairs
  Toolchain_gcc gcc;
  ExecutableTarget_gcc hello_world("hello_world", gcc, "");

  // Select your builds and tests using the .toml files
  Reg::Toolchain(arg_gcc.state)
      .Func([&]() { gcc.Verify(); })
      .Build(hello_world_build_cb, hello_world)
      .Test("{executable}", hello_world);

  // 6. Build and Test Target
  Reg::Run();

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
