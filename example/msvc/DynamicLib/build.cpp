#include <filesystem>
#include <iostream>

#include "buildcc.h"
#include "constants.h"

using namespace buildcc;

int main(void) {
  // Environment is meant to define
  // 1. Project Root path i.e all files and targets will be added relative to
  // this path
  // 2. Intermediate build folder i.e all intermediate generated files should go
  // here
  env::init(BUILD_SCRIPT_SOURCE, BUILD_SCRIPT_FOLDER);
  env::set_log_level(env::LogLevel::Debug);

  Toolchain_msvc msvc;

  DynamicTarget_msvc dynamictarget("librandom", msvc, "");
  dynamictarget.AddSource("src/random.cpp");
  dynamictarget.AddIncludeDir("include", true);
  dynamictarget.Build();

  ExecutableTarget_msvc target_msvc("Simple", msvc, "");
  target_msvc.AddSource("src/main.cpp");
  target_msvc.AddIncludeDir("include", true);

  // Method 1
  target_msvc.AddLibDep(dynamictarget);

  // Method 2
  // target_msvc.AddLibDep("librandom.lib");
  // target_msvc.AddLibDir(dynamictarget.GetTargetPath().parent_path());
  target_msvc.Build();

  // Manually setup your dependencies
  tf::Executor executor;
  tf::Taskflow taskflow;

  auto dynamictargetTask = taskflow.composed_of(dynamictarget.GetTaskflow());
  auto target_msvcTask = taskflow.composed_of(target_msvc.GetTaskflow());
  target_msvcTask.succeed(dynamictargetTask);

  executor.run(taskflow);
  executor.wait_for_all();

  if (target_msvc.IsBuilt()) {
    fs::copy(dynamictarget.GetTargetPath().string() + ".dll",
             target_msvc.GetTargetPath().parent_path() / "librandom.lib.dll");
  }

  plugin::ClangCompileCommands({&dynamictarget, &target_msvc}).Generate();

  taskflow.dump(std::cout);

  return 0;
}
