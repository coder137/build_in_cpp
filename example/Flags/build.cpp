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

  // CppTarget
  {
    ExecutableTarget target(
        "CppFlags.exe", base::Toolchain("gcc", "as", "gcc", "g++", "ar", "ld"),
        "files");

    target.SetDefaultLinker("cpp");
    target.AddSource("main.cpp", "src");
    target.AddSource("src/random.cpp");
    target.AddIncludeDir("include");
    target.AddPreprocessorFlag("-DRANDOM=1");
    target.AddCppCompileFlag("-Wall");
    target.AddCppCompileFlag("-Werror");
    target.AddLinkFlag("-lm");
    target.Build();
  }

  // CTarget
  {
    ExecutableTarget target(
        "CFlags.exe", base::Toolchain("gcc", "as", "gcc", "g++", "ar", "ld"),
        "files");
    target.SetDefaultLinker("c");
    target.AddSource("main.c", "src");
    target.AddPreprocessorFlag("-DRANDOM=1");
    target.AddCCompileFlag("-Wall");
    target.AddCCompileFlag("-Werror");
    target.AddLinkFlag("-lm");
    target.Build();
  }

  return 0;
}
