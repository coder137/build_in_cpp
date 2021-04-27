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

  base::Toolchain gcc("gcc", "as", "gcc", "g++", "ar", "ld");

  StaticTarget_gcc randomLib("libran.a", gcc, "files");
  randomLib.AddSource("src/random.cpp");
  randomLib.AddHeader("include/random.h");
  randomLib.AddIncludeDir("include");
  randomLib.Build();

  ExecutableTarget_gcc target("statictest.exe", gcc, "files");
  target.AddSource("main.cpp", "src");
  target.AddIncludeDir("include");
  target.AddLibDep(randomLib);
  target.Build();

  return 0;
}
