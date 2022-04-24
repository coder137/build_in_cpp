#include "buildcc.h"

#include "fmt/format.h"

using namespace buildcc;

constexpr const char *const EXE = "build";

// Function Prototypes
static void clean_cb();
static void cppflags_build_cb(BaseTarget &cppflags);
static void cflags_build_cb(BaseTarget &cflags);

int main(int argc, char **argv) {
  // Get arguments
  ArgToolchain arg_gcc;
  ArgToolchain arg_msvc;
  Args::Init()
      .AddToolchain("gcc", "Generic gcc toolchain", arg_gcc)
      .AddToolchain("msvc", "Generic msvc toolchain", arg_msvc)
      .Parse(argc, argv);

  // Initialize your environment
  Reg::Init();

  // Pre-build steps
  Reg::Call(Args::Clean()).Func(clean_cb);

  // Build steps
  // Explicit toolchain - target pairs
  Toolchain_gcc gcc;
  ExecutableTarget_gcc g_cppflags("cppflags", gcc, "files");
  ExecutableTarget_gcc g_cflags("cflags", gcc, "files");

  Reg::Toolchain(arg_gcc.state)
      .Build(cppflags_build_cb, g_cppflags)
      .Build(cflags_build_cb, g_cflags)
      .Test("{executable}", g_cppflags)
      .Test("{executable}", g_cflags);

  Toolchain_msvc msvc;
  ExecutableTarget_msvc m_cppflags("cppflags", msvc, "files");
  ExecutableTarget_msvc m_cflags("cflags", msvc, "files");
  Reg::Toolchain(arg_msvc.state)
      .Build(cppflags_build_cb, m_cppflags)
      .Build(cflags_build_cb, m_cflags)
      .Test("{executable}", m_cppflags)
      .Test("{executable}", m_cflags);

  Reg::Run();

  // Post Build steps

  // - Clang Compile Commands
  plugin::ClangCompileCommands({&g_cflags, &g_cppflags}).Generate();

  // - Plugin Graph
  std::string output = Reg::GetTaskflow().dump();
  const bool saved = env::save_file("graph.dot", output, false);
  env::assert_fatal(saved, "Could not save graph.dot file");

  return 0;
}

static void clean_cb() {
  env::log_info(EXE, fmt::format("Cleaning {}", Project::GetBuildDir()));
  fs::remove_all(Project::GetBuildDir());
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
