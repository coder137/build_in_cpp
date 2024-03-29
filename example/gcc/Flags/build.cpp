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
  Project::Init(BUILD_ROOT, BUILD_INTERMEDIATE_DIR);
  env::set_log_level(env::LogLevel::Trace);

  Toolchain_gcc gcc;

  // CppTarget
  ExecutableTarget_gcc cpptarget("CppFlags", gcc, "files");
  cpptarget.AddSource("main.cpp", "src");
  cpptarget.AddSource("src/random.cpp");
  cpptarget.AddHeader("include/random.h");
  cpptarget.AddIncludeDir("include");
  cpptarget.AddPreprocessorFlag("-DRANDOM=1");
  cpptarget.AddCppCompileFlag("-Wall");
  cpptarget.AddCppCompileFlag("-Werror");
  cpptarget.AddLinkFlag("-lm");
  cpptarget.Build();

  // CTarget
  ExecutableTarget_gcc ctarget("CFlags", gcc, "files");
  ctarget.AddSource("main.c", "src");
  ctarget.AddPreprocessorFlag("-DRANDOM=1");
  ctarget.AddCCompileFlag("-Wall");
  ctarget.AddCCompileFlag("-Werror");
  ctarget.AddLinkFlag("-lm");
  ctarget.Build();

  // Run
  tf::Executor executor;
  tf::Taskflow taskflow;
  taskflow.composed_of(cpptarget.GetTaskflow());
  taskflow.composed_of(ctarget.GetTaskflow());
  executor.run(taskflow);
  executor.wait_for_all();

  taskflow.dump(std::cout);

  return 0;
}
