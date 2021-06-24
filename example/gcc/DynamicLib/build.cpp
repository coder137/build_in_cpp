#include <filesystem>
#include <iostream>

#include "buildcc.h"
#include "constants.h"

using namespace buildcc;

// This example contains both OS Hosts
// - Windows MSYS GCC 10.2.0
// - Linux GCC 9.3.0
// * Comment out one of the examples
int main(void) {
  // Environment is meant to define
  // 1. Project Root path i.e all files and targets will be added relative to
  // this path
  // 2. Intermediate build folder i.e all intermediate generated files should go
  // here
  env::init(BUILD_ROOT, BUILD_INTERMEDIATE_DIR);
  env::set_log_level(env::LogLevel::Trace);

  base::Toolchain gcc(base::Toolchain::Id::Gcc, "gcc", "as", "gcc", "g++", "ar",
                      "ld");

  const std::string_view dynl_ext =
      Target_generic::Extension(base::TargetType::DynamicLibrary, gcc.GetId());
  DynamicTarget_gcc randomDynLib(fmt::format("librandyn{}", dynl_ext), gcc,
                                 "files");

  const std::string_view ex_ext =
      Target_generic::Extension(base::TargetType::Executable, gcc.GetId());
  ExecutableTarget_gcc target(fmt::format("dynamictest{}", ex_ext), gcc,
                              "files");

  randomDynLib.AddSource("src/random.cpp");
  randomDynLib.AddHeader("include/random.h");
  randomDynLib.AddIncludeDir("include");
  randomDynLib.Build();

  target.AddSource("main.cpp", "src");
  target.AddIncludeDir("include");

  // * Method 1
  // NOTE, Use buildcc built targets
  target.AddLibDep(randomDynLib);

  // * Method 2, External lib
  // target.AddLibDirAbsolute(randomDynLib.GetTargetIntermediateDir());
  // target.AddLibDep("-lrandyn");

  // OS Specific
  if constexpr (env::is_linux()) {
    target.AddLinkFlag("-Wl,-rpath=" +
                       randomDynLib.GetTargetIntermediateDir().string());
  }
  target.Build();

  tf::Executor executor;
  tf::Taskflow taskflow;
  auto randomDynLibTask = taskflow.composed_of(randomDynLib.GetTaskflow());
  auto targetTask = taskflow.composed_of(target.GetTaskflow());

  targetTask.succeed(randomDynLibTask);

  executor.run(taskflow);
  executor.wait_for_all();

  // Post Build step
  if constexpr (env::is_win()) {
    if (target.FirstBuild() || target.Rebuild()) {
      fs::path copy_to_path =
          target.GetTargetIntermediateDir() / randomDynLib.GetName();
      fs::remove(copy_to_path);
      fs::copy(randomDynLib.GetTargetPath(), copy_to_path);
    }
  }

  // Dump .dot output
  taskflow.dump(std::cout);

  return 0;
}
