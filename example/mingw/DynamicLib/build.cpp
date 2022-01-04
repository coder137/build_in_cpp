#include <filesystem>
#include <iostream>

#include "buildcc.h"
#include "constants.h"

using namespace buildcc;

// - Windows MinGW 10.2.0
int main(void) {
  // Environment is meant to define
  // 1. Project Root path i.e all files and targets will be added relative to
  // this path
  // 2. Intermediate build folder i.e all intermediate generated files should go
  // here
  env::init(BUILD_ROOT, BUILD_INTERMEDIATE_DIR);
  env::set_log_level(env::LogLevel::Trace);

  Toolchain_mingw mingw;

  DynamicTarget_mingw dynamictarget("librandom", mingw, "files");
  dynamictarget.AddSource("src/random.cpp");
  dynamictarget.AddHeader("include/random.h");
  dynamictarget.AddIncludeDir("include");
  dynamictarget.Build();

  ExecutableTarget_mingw target("dynamictest", mingw, "files");
  target.AddSource("main.cpp", "src");
  target.AddIncludeDir("include");

  // * Method 1
  // NOTE, Use buildcc built targets
  target.AddLibDep(dynamictarget);

  // * Method 2, External lib
  // target.AddLibDirAbsolute(dynamictarget.GetTargetBuildDir());
  // target.AddLibDep("-lrandom");

  target.Build();

  tf::Executor executor;
  tf::Taskflow taskflow;
  auto dynamictargetTask = taskflow.composed_of(dynamictarget.GetTaskflow());
  auto targetTask = taskflow.composed_of(target.GetTaskflow());

  targetTask.succeed(dynamictargetTask);

  executor.run(taskflow);
  executor.wait_for_all();

  // Post Build step
  if (target.IsBuilt()) {
    fs::path copy_to_path =
        target.GetTargetBuildDir() / dynamictarget.GetTargetPath().filename();
    fs::remove_all(copy_to_path);
    fs::copy(dynamictarget.GetTargetPath(), copy_to_path);
  }

  // Dump .dot output
  taskflow.dump(std::cout);

  return 0;
}
