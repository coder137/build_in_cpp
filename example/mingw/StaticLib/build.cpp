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
  env::init(BUILD_ROOT, BUILD_INTERMEDIATE_DIR);
  env::set_log_level(env::LogLevel::Trace);

  Toolchain_mingw mingw;

  StaticTarget_mingw statictarget("librandom", mingw, "files");
  statictarget.AddSource("src/random.cpp");
  statictarget.AddHeader("include/random.h");
  statictarget.AddIncludeDir("include");
  statictarget.Build();

  ExecutableTarget_mingw exetarget("statictest", mingw, "files");
  exetarget.AddSource("main.cpp", "src");
  exetarget.AddIncludeDir("include");

  // * Method 1
  // NOTE, Use buildcc built targets
  exetarget.AddLibDep(statictarget);

  // * Method 2
  // NOTE, This should be an absolute path since we could also be referencing
  // external libs that are not relative to this project
  // exetarget.AddLibDirAbsolute(statictarget.GetTargetPath().parent_path());
  // exetarget.AddLibDep("-lrandom");

  exetarget.Build();

  // Run
  tf::Executor executor;
  tf::Taskflow taskflow;

  tf::Task statictargetTask = taskflow.composed_of(statictarget.GetTaskflow());
  tf::Task exetargetTask = taskflow.composed_of(exetarget.GetTaskflow());

  // Set dependency
  exetargetTask.succeed(statictargetTask);

  // Run
  executor.run(taskflow);
  executor.wait_for_all();

  // Dump .dot out
  taskflow.dump(std::cout);

  return 0;
}
