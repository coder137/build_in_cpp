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
  Project::Init(BUILD_SCRIPT_SOURCE, BUILD_SCRIPT_FOLDER);
  env::set_log_level(env::LogLevel::Debug);

  Toolchain_msvc msvc;

  DynamicTarget_msvc dynamictarget("librandom", msvc, "");
  dynamictarget.AddSource("src/random.cpp");
  dynamictarget.AddIncludeDir("include", true);
  dynamictarget.Build();

  ExecutableTarget_msvc exetarget("Simple", msvc, "");
  exetarget.AddSource("src/main.cpp");
  exetarget.AddIncludeDir("include", true);

  // Method 1
  exetarget.AddLibDep(dynamictarget);

  // Method 2
  // exetarget.AddLibDep("librandom.lib");
  // exetarget.AddLibDir(dynamictarget.GetTargetPath().parent_path());
  exetarget.Build();

  // Manually setup your dependencies
  tf::Executor executor;
  tf::Taskflow taskflow;

  auto dynamictargetTask = taskflow.composed_of(dynamictarget.GetTaskflow());
  auto exetargetTask = taskflow.composed_of(exetarget.GetTaskflow());
  exetargetTask.succeed(dynamictargetTask);

  executor.run(taskflow);
  executor.wait_for_all();

  if (exetarget.IsBuilt()) {
    fs::copy(dynamictarget.GetDllPath(),
             exetarget.GetTargetPath().parent_path() /
                 dynamictarget.GetDllPath().filename());
  }

  plugin::ClangCompileCommands({&dynamictarget, &exetarget}).Generate();

  taskflow.dump(std::cout);

  return 0;
}
