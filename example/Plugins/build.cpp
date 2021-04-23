#include <filesystem>
#include <iostream>

#include "buildcc.h"
#include "constants.h"

#include "clang_compile_commands.h"

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
  base::Toolchain gcc("gcc", "as", "gcc", "g++", "ar", "ld");
  ExecutableTarget cppflags("CppFlags.exe", gcc, "files");

  cppflags.AddSource("main.cpp", "src");
  cppflags.AddSource("src/random.cpp");
  cppflags.AddIncludeDir("include");
  cppflags.AddPreprocessorFlag("-DRANDOM=1");
  cppflags.AddCppCompileFlag("-Wall");
  cppflags.AddCppCompileFlag("-Werror");
  cppflags.AddLinkFlag("-lm");
  cppflags.Build();

  // CTarget
  ExecutableTarget cflags("CFlags.exe", gcc, "files");
  cflags.AddSource("main.c", "src");
  cflags.AddPreprocessorFlag("-DRANDOM=1");
  cflags.AddCCompileFlag("-Wall");
  cflags.AddCCompileFlag("-Werror");
  cflags.AddLinkFlag("-lm");
  cflags.Build();

  // Clang compile command db test
  plugin::ClangCompileCommands compile_commands({&cppflags, &cflags});
  compile_commands.Generate();

  return 0;
}
