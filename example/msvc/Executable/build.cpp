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

  ExecutableTarget_msvc target_msvc("Simple", msvc, "");
  target_msvc.GlobSources("src");
  target_msvc.AddIncludeDir("include", true);
  target_msvc.Build();

  plugin::ClangCompileCommands({&target_msvc}).Generate();

  tf::Executor executor;
  executor.run(target_msvc.GetTaskflow());
  executor.wait_for_all();

  target_msvc.GetTaskflow().dump(std::cout);

  return 0;
}
