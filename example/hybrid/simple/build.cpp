#include "buildcc.h"

#include "fmt/format.h"

#include "flatbuffers/util.h"

using namespace buildcc;

constexpr const char *const EXE = "build";

// Function Prototypes
static void clean_cb();
static void gcppflags_build_cb(base::Target &g_cppflags);
static void gcflags_build_cb(base::Target &g_cflags);
static void mcppflags_build_cb(base::Target &m_cppflags);
static void mcflags_build_cb(base::Target &m_cflags);

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

  ExecutableTarget_msvc m_cppflags("MCppFlags.exe", msvc, "files");
  ExecutableTarget_msvc m_cflags("MCFlags.exe", msvc, "files");

  reg.Build(args.GetGccState(), g_cppflags, gcppflags_build_cb);
  reg.Build(args.GetGccState(), g_cflags, gcflags_build_cb);
  reg.Build(args.GetMsvcState(), m_cppflags, mcppflags_build_cb);
  reg.Build(args.GetMsvcState(), m_cflags, mcflags_build_cb);

  // 5. Test steps

  // NOTE, For now they are just dummy callbacks
  reg.Test(args.GetGccState(), g_cppflags, [](base::Target &target) {});
  reg.Test(args.GetGccState(), g_cflags, [](base::Target &target) {});
  reg.Test(args.GetMsvcState(), m_cppflags, [](base::Target &target) {});
  reg.Test(args.GetMsvcState(), m_cflags, [](base::Target &target) {});

  // 6. Build Target
  reg.RunBuild();

  // 7. Test Target
  reg.RunTest();

  // 8. Post Build steps

  // - Clang Compile Commands
  plugin::ClangCompileCommands({&g_cflags, &g_cppflags, &m_cflags, &m_cppflags})
      .Generate();

  // - Plugin Graph
  std::string output = reg.GetTaskflow().dump();
  const bool saved = flatbuffers::SaveFile("graph.dot", output, false);
  env::assert_fatal(saved, "Could not save graph.dot file");

  return 0;
}

static void clean_cb() {
  env::log_info(
      EXE, fmt::format("Cleaning {}", env::get_project_build_dir().string()));
  fs::remove_all(env::get_project_build_dir());
}

static void gcppflags_build_cb(base::Target &g_cppflags) {
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
}

static void gcflags_build_cb(base::Target &g_cflags) {
  // GCC CFlags
  g_cflags.AddSource("main.c", "src");
  g_cflags.AddPreprocessorFlag("-DRANDOM=1");
  g_cflags.AddCCompileFlag("-Wall");
  g_cflags.AddCCompileFlag("-Werror");
  g_cflags.AddLinkFlag("-lm");
  g_cflags.Build();
}

static void mcppflags_build_cb(base::Target &m_cppflags) {
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
}

static void mcflags_build_cb(base::Target &m_cflags) {
  // GCC CFlags
  m_cflags.AddSource("main.c", "src");
  m_cflags.AddPreprocessorFlag("/DRANDOM=1");
  m_cflags.AddCCompileFlag("/W4");
  m_cflags.AddCCompileFlag("/nologo");
  m_cflags.AddLinkFlag("/nologo");
  m_cflags.Build();
}
