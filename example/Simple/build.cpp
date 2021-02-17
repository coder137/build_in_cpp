#include <filesystem>
#include <iostream>

#include "buildcc.h"
#include "constants.h"

using namespace buildcc;

int main(void) {
  Target target("Simple.exe", TargetType::Executable,
                Toolchain("gcc", "gcc", "g++"), BUILD_SCRIPT_SOURCE);
  target.AddSource("main.cpp");
  target.Build();
  return 0;
}
