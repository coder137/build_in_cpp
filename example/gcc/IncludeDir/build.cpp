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

  ExecutableTarget_gcc target("IncludeDir", gcc, "files");
  target.AddSource("main.cpp", "src");
  target.AddSource("src/random.cpp");
  target.AddHeader("include/random.h");
  target.AddIncludeDir("include");
  target.Build();

  // Run
  tf::Executor executor;
  executor.run(target.GetTaskflow());
  executor.wait_for_all();

  // .dot output
  target.GetTaskflow().dump(std::cout);

  return 0;
}
