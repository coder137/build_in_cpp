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
  env::set_log_level(env::LogLevel::Trace);

  Toolchain_msvc msvc;

  StaticTarget_msvc statictarget_msvc("librandom.lib", msvc, "");
  statictarget_msvc.AddSource("src/random.cpp");
  statictarget_msvc.AddIncludeDir("include", true);
  statictarget_msvc.AddCppCompileFlag("/EHsc");
  statictarget_msvc.AddCppCompileFlag("/nologo");
  statictarget_msvc.AddLinkFlag("/nologo");
  statictarget_msvc.Build();

  ExecutableTarget_msvc target_msvc("Simple.exe", msvc, "");
  target_msvc.AddSource("src/main.cpp");
  target_msvc.AddIncludeDir("include", true);

  // Method 1
  // target_msvc.AddLibDep(statictarget_msvc);

  // Method 2
  target_msvc.AddLibDep("librandom.lib");
  target_msvc.AddLibDir(statictarget_msvc.GetTargetPath().parent_path());

  target_msvc.AddCppCompileFlag("/EHsc");
  target_msvc.AddCppCompileFlag("/nologo");
  target_msvc.AddLinkFlag("/nologo");
  target_msvc.Build();

  plugin::ClangCompileCommands({&target_msvc}).Generate();

  tf::Executor executor;
  tf::Taskflow taskflow;
  auto statictarget_msvcTask =
      taskflow.composed_of(statictarget_msvc.GetTaskflow());
  auto target_msvcTask = taskflow.composed_of(target_msvc.GetTaskflow());
  target_msvcTask.succeed(statictarget_msvcTask);

  executor.run(taskflow);
  executor.wait_for_all();

  taskflow.dump(std::cout);

  return 0;
}
