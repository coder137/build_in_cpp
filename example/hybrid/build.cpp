#include "buildcc.h"
#include "clang_compile_commands.h"

#include "fmt/format.h"

using namespace buildcc;

constexpr const char *const EXE = "build";

int main(int argc, char **argv) {
  // 1. Get arguments
  Args args;
  args.Parse(argc, argv);

  // 2. Initialize your environment
  env::init(fs::current_path() / args.GetProjectRootDir(),
            fs::current_path() / args.GetProjectBuildDir());
  env::set_log_level(args.GetLogLevel());

  // 3. Pre-build steps
  if (args.Clean()) {
    env::log_info(
        EXE, fmt::format("Cleaning {}", env::get_project_build_dir().string()));
    fs::remove_all(env::get_project_build_dir());
  }

  // 4. Build steps
  Toolchain_gcc gcc;
  Toolchain_msvc msvc;

  ExecutableTarget_gcc g_cppflags("GCppFlags.exe", gcc, "files");
  ExecutableTarget_gcc g_cflags("GCFlags.exe", gcc, "files");
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
  if (args.GetGccToolchain().build && args.GetGccToolchain().test) {
    std::string cppflags_loc = g_cppflags.GetTargetPath().string();
    env::log_info(EXE, fmt::format("Testing {}", cppflags_loc.c_str()));

    // system ...

    std::string cflags_loc = g_cflags.GetTargetPath().string();
    env::log_info(EXE, fmt::format("Testing {}", cflags_loc.c_str()));
    // system ...
  }

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

  return 0;
}
