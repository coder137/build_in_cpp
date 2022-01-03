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

  StaticTarget_msvc statictarget("librandom", msvc, "");
  statictarget.AddSource("src/random.cpp");
  statictarget.AddIncludeDir("include", true);
  statictarget.Build();

  ExecutableTarget_msvc exetarget("Simple", msvc, "");
  exetarget.AddSource("src/main.cpp");
  exetarget.AddIncludeDir("include", true);

  // Method 1
  exetarget.AddLibDep(statictarget);

  // Method 2
  // exetarget.AddLibDep("librandom.lib");
  // exetarget.AddLibDir(statictarget.GetTargetPath().parent_path());
  exetarget.Build();

  plugin::ClangCompileCommands({&exetarget}).Generate();

  // Manually setup your dependencies
  tf::Executor executor;
  tf::Taskflow taskflow;
  auto statictargetTask = taskflow.composed_of(statictarget.GetTaskflow());
  auto exetargetTask = taskflow.composed_of(exetarget.GetTaskflow());
  exetargetTask.succeed(statictargetTask);

  executor.run(taskflow);
  executor.wait_for_all();

  taskflow.dump(std::cout);

  return 0;
}
