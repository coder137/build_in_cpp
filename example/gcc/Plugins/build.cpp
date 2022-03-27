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
  ExecutableTarget_gcc cppflags("CppFlags", gcc, "files");
  cppflags.AddSource("main.cpp", "src");
  cppflags.AddSource("src/random.cpp");
  cppflags.AddHeader("include/random.h");
  cppflags.AddIncludeDir("include");
  cppflags.AddPreprocessorFlag("-DRANDOM=1");
  cppflags.AddCppCompileFlag("-Wall");
  cppflags.AddCppCompileFlag("-Werror");
  cppflags.AddLinkFlag("-lm");
  cppflags.Build();

  // CTarget
  ExecutableTarget_gcc cflags("CFlags", gcc, "files");
  cflags.AddSource("main.c", "src");
  cflags.AddPreprocessorFlag("-DRANDOM=1");
  cflags.AddCCompileFlag("-Wall");
  cflags.AddCCompileFlag("-Werror");
  cflags.AddLinkFlag("-lm");
  cflags.Build();

  tf::Executor executor;
  tf::Taskflow taskflow;
  taskflow.composed_of(cflags.GetTaskflow());
  taskflow.composed_of(cppflags.GetTaskflow());
  executor.run(taskflow);
  executor.wait_for_all();

  // Clang compile command db test
  plugin::ClangCompileCommands({&cppflags, &cflags}).Generate();

  // Dot Dump
  // TODO, Make this into a plugin
  taskflow.dump(std::cout);

  return 0;
}
