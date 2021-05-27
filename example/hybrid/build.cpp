#include "buildcc.h"
#include "clang_compile_commands.h"

#include "fmt/format.h"

#include "taskflow/taskflow.hpp"

#include "flatbuffers/util.h"

#include "assert_fatal.h"

using namespace buildcc;

constexpr const char *const EXE = "build";

static void clean_cb() {
  env::log_info(
      EXE, fmt::format("Cleaning {}", env::get_project_build_dir().string()));
  fs::remove_all(env::get_project_build_dir());
}

int main(int argc, char **argv) {
  // 1. Get arguments
  Args args;
  args.Parse(argc, argv);

  // 2. Initialize your environment
  Register reg(args);
  reg.Env();

  // 3. Pre-build steps
  reg.Clean(clean_cb);

  // 4. Build steps
  Toolchain_gcc gcc;
  Toolchain_msvc msvc;

  ExecutableTarget_gcc g_cppflags("GCppFlags.exe", gcc, "files");
  ExecutableTarget_gcc g_cflags("GCFlags.exe", gcc, "files");

  // TODO, Register.Toolchain
  if (args.GetGccToolchain().build) {
    // GCC CppFlags
    g_cppflags.AddSource("main.cpp", "src");
    g_cppflags.AddSource("src/random.cpp");

    g_cppflags.AddHeader("include/random.h");
    g_cppflags.AddIncludeDir("include");

    g_cppflags.AddPreprocessorFlag("-DRANDOM=1");
    g_cppflags.AddCppCompileFlag("-Wall");
    g_cppflags.AddCppCompileFlag("-Werror");
    g_cppflags.AddLinkFlag("-lm");
    g_cppflags.Build();

    // GCC CFlags
    g_cflags.AddSource("main.c", "src");
    g_cflags.AddPreprocessorFlag("-DRANDOM=1");
    g_cflags.AddCCompileFlag("-Wall");
    g_cflags.AddCCompileFlag("-Werror");
    g_cflags.AddLinkFlag("-lm");
    g_cflags.Build();
  }

  ExecutableTarget_msvc m_cppflags("MCppFlags.exe", msvc, "files");
  ExecutableTarget_msvc m_cflags("MCFlags.exe", msvc, "files");

  // TODO, Register.Toolchain
  if (args.GetMsvcToolchain().build) {
    // GCC CppFlags
    m_cppflags.AddSource("main.cpp", "src");
    m_cppflags.AddSource("src/random.cpp");

    m_cppflags.AddHeader("include/random.h");
    m_cppflags.AddIncludeDir("include");

    m_cppflags.AddPreprocessorFlag("/DRANDOM=1");
    m_cppflags.AddCppCompileFlag("/W4");
    m_cppflags.AddCppCompileFlag("/nologo");
    m_cppflags.AddCppCompileFlag("/EHsc");
    m_cppflags.AddLinkFlag("/nologo");
    m_cppflags.Build();

    // GCC CFlags
    m_cflags.AddSource("main.c", "src");
    m_cflags.AddPreprocessorFlag("/DRANDOM=1");
    m_cflags.AddCCompileFlag("/W4");
    m_cflags.AddCCompileFlag("/nologo");
    m_cflags.AddLinkFlag("/nologo");
    m_cflags.Build();
  }

  // 5. Test Steps
  // TODO, Register.Test();
  if (args.GetGccToolchain().build && args.GetGccToolchain().test) {
    std::string cppflags_loc = g_cppflags.GetTargetPath().string();
    env::log_info(EXE, fmt::format("Testing {}", cppflags_loc.c_str()));

    // system ...

    std::string cflags_loc = g_cflags.GetTargetPath().string();
    env::log_info(EXE, fmt::format("Testing {}", cflags_loc.c_str()));
    // system ...
  }

  // TODO, Register.Test();
  if (args.GetMsvcToolchain().build && args.GetMsvcToolchain().test) {
    std::string cppflags_loc = m_cppflags.GetTargetPath().string();
    env::log_info(EXE, fmt::format("Testing {}", cppflags_loc.c_str()));
    // system ...

    std::string cflags_loc = m_cflags.GetTargetPath().string();
    env::log_info(EXE, fmt::format("Testing {}", cflags_loc.c_str()));
    // system ...
  }

  // 6. Post Build Tools
  plugin::ClangCompileCommands({&g_cflags, &g_cppflags, &m_cflags, &m_cppflags})
      .Generate();

  // TODO, Register.BuildTargets()
  tf::Executor executor;
  tf::Taskflow maintf;
  maintf.name("Targets");
  maintf.composed_of(g_cppflags.GetTaskflow()).name("Task");
  maintf.composed_of(g_cflags.GetTaskflow()).name("Task");
  maintf.composed_of(m_cppflags.GetTaskflow()).name("Task");
  maintf.composed_of(m_cflags.GetTaskflow()).name("Task");
  executor.run(maintf).get();

  // TODO, Register.RunTests()

  // TODO, Plugin Graph
  std::string output = maintf.dump();
  const bool saved = flatbuffers::SaveFile("graph.dot", output, false);
  env::assert_fatal(saved, "Could not save graph.dot file");

  return 0;
}
