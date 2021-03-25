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

  Target target("Simple.exe", TargetType::Executable,
                Toolchain("gcc", "gcc", "g++"), "files");
  target.AddSource("main.cpp", "src");
  target.AddSource("src/random.cpp");
  target.AddIncludeDir("include");
  target.Build();
  return 0;
}
