#include "buildcc.h"
#include "constants.h"

using namespace buildcc;

constexpr const char *const EXE = "build";

// Function Prototypes
static void cppflags_build_cb(BaseTarget &cppflags);
static void cflags_build_cb(BaseTarget &cflags);

int main() {
  // Environment is meant to define
  // 1. Project Root path i.e all files and targets will be added relative to
  // this path
  // 2. Intermediate build folder i.e all intermediate generated files should go
  // here
  env::init(BUILD_ROOT, BUILD_INTERMEDIATE_DIR);
  env::set_log_level(env::LogLevel::Trace);

  Toolchain_mingw mingw;

  ExecutableTarget_mingw g_cppflags("cppflags", mingw, "files");
  cppflags_build_cb(g_cppflags);

  ExecutableTarget_mingw g_cflags("cflags", mingw, "files");
  cflags_build_cb(g_cflags);

  tf::Executor executor;
  tf::Taskflow taskflow;

  taskflow.composed_of(g_cppflags.GetTaskflow());
  taskflow.composed_of(g_cflags.GetTaskflow());
  executor.run(taskflow);
  executor.wait_for_all();

  return 0;
}

static void cppflags_build_cb(BaseTarget &cppflags) {
  cppflags.AddSource("main.cpp", "src");
  cppflags.AddSource("random.cpp", "src");
  cppflags.AddIncludeDir("include", true);

  cppflags.AddPch("pch/pch_cpp.h");
  cppflags.AddPch("pch/pch_c.h");
  cppflags.AddIncludeDir("pch", true);

  // Toolchain specific code goes here
  switch (cppflags.GetToolchain().GetId()) {
  case ToolchainId::Gcc: {
    cppflags.AddPreprocessorFlag("-DRANDOM=1");
    cppflags.AddCppCompileFlag("-Wall");
    cppflags.AddCppCompileFlag("-Werror");
    cppflags.AddLinkFlag("-lm");
    break;
  }
  case ToolchainId::Msvc: {
    cppflags.AddPreprocessorFlag("/DRANDOM=1");
    cppflags.AddCppCompileFlag("/W4");
    break;
  }
  default:
    break;
  }

  cppflags.Build();
}

static void cflags_build_cb(BaseTarget &cflags) {
  cflags.AddSource("main.c", "src");

  cflags.AddPch("pch/pch_c.h");
  cflags.AddIncludeDir("pch", false);
  cflags.AddHeader("pch/pch_c.h");

  // Toolchain specific code goes here
  switch (cflags.GetToolchain().GetId()) {
  case ToolchainId::Gcc: {
    cflags.AddPreprocessorFlag("-DRANDOM=1");
    cflags.AddCCompileFlag("-Wall");
    cflags.AddCCompileFlag("-Werror");
    cflags.AddLinkFlag("-lm");
    break;
  }
  case ToolchainId::Msvc: {
    cflags.AddPreprocessorFlag("/DRANDOM=1");
    cflags.AddCCompileFlag("/W4");
    break;
  }
  default:
    break;
  }

  cflags.Build();
}
