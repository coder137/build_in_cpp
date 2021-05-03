#include <filesystem>
#include <iostream>

#include "buildcc.h"
#include "constants.h"

using namespace buildcc;

// This example contains both OS Hosts
// - Windows MSYS GCC 10.2.0
// - Linux GCC 9.3.0
// * Comment out one of the examples
int main(void) {
  // Environment is meant to define
  // 1. Project Root path i.e all files and targets will be added relative to
  // this path
  // 2. Intermediate build folder i.e all intermediate generated files should go
  // here
  env::init(BUILD_ROOT, BUILD_INTERMEDIATE_DIR);
  env::set_log_level(env::LogLevel::Trace);

  base::Toolchain gcc("gcc", "as", "gcc", "g++", "ar", "ld");

  // Linux GCC
  {
    DynamicTarget_gcc randomDynLib("librandyn.so", gcc, "files");
    randomDynLib.AddSource("src/random.cpp");
    randomDynLib.AddHeader("include/random.h");
    randomDynLib.AddIncludeDir("include");
    randomDynLib.Build();

    ExecutableTarget_gcc target("dynamictest.exe", gcc, "files");
    target.AddSource("main.cpp", "src");
    target.AddIncludeDir("include");

    // * Method 1
    // NOTE, Use buildcc built targets
    // target.AddLibDep(randomDynLib);

    // * Method 2, External lib
    target.AddLibDirAbsolute(randomDynLib.GetTargetIntermediateDir());
    target.AddLibDep("-lrandyn");
    target.AddLinkFlag("-Wl,-rpath=" +
                       randomDynLib.GetTargetIntermediateDir().string());

    target.Build();
  }

  // MingW GCC
  {
    DynamicTarget_gcc randomDynLib("librandyn.dll", gcc, "files");
    randomDynLib.AddSource("src/random.cpp");
    randomDynLib.AddHeader("include/random.h");
    randomDynLib.AddIncludeDir("include");
    randomDynLib.Build();

    ExecutableTarget_gcc target("dynamictest.exe", gcc, "files");
    target.AddSource("main.cpp", "src");
    target.AddIncludeDir("include");

    // * Method 1
    // NOTE, Use buildcc built targets
    // target.AddLibDep(randomDynLib);

    // * Method 2, External lib
    target.AddLibDirAbsolute(randomDynLib.GetTargetIntermediateDir());
    target.AddLibDep("-lrandyn");

    target.Build();

    if (target.FirstBuild() || target.Rebuild()) {
      fs::path copy_to_path =
          target.GetTargetIntermediateDir() / randomDynLib.GetName();
      fs::remove(copy_to_path);
      fs::copy(randomDynLib.GetTargetPath(), copy_to_path);
    }
  }

  return 0;
}
