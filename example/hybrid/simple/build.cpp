#include "buildcc.h"

#include "fmt/format.h"

using namespace buildcc;

constexpr const char *const EXE = "build";

// Function Prototypes
static void clean_cb();
static void cppflags_build_cb(base::Target &cppflags);
static void cflags_build_cb(base::Target &cflags);

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
  // Explicit toolchain - target pairs
  Toolchain_gcc gcc;
  Toolchain_msvc msvc;

  ExecutableTarget_gcc g_cppflags("cppflags", gcc, "files");
  ExecutableTarget_msvc m_cppflags("cppflags", msvc, "files");
  ExecutableTarget_gcc g_cflags("cflags", gcc, "files");
  ExecutableTarget_msvc m_cflags("cflags", msvc, "files");

  // Select your builds and tests using the .toml files
  reg.Build(args.GetGccState(), g_cppflags, cppflags_build_cb);
  reg.Build(args.GetMsvcState(), m_cppflags, cppflags_build_cb);
  reg.Build(args.GetGccState(), g_cflags, cflags_build_cb);
  reg.Build(args.GetMsvcState(), m_cflags, cflags_build_cb);

  // 5. Test steps
  // NOTE, For now they are just dummy callbacks
  reg.Test(args.GetGccState(), g_cppflags, [](base::Target &target) {});
  reg.Test(args.GetMsvcState(), m_cppflags, [](base::Target &target) {});
  reg.Test(args.GetGccState(), g_cflags, [](base::Target &target) {});
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
  const bool saved = env::SaveFile("graph.dot", output, false);
  env::assert_fatal(saved, "Could not save graph.dot file");

  return 0;
}

static void clean_cb() {
  env::log_info(
      EXE, fmt::format("Cleaning {}", env::get_project_build_dir().string()));
  fs::remove_all(env::get_project_build_dir());
}

static void cppflags_build_cb(base::Target &cppflags) {
  cppflags.AddSource("main.cpp", "src");
  cppflags.AddSource("src/random.cpp");
  cppflags.AddIncludeDir("include", true);

  // Toolchain specific code goes here
  switch (cppflags.GetToolchain().GetId()) {
  case base::Toolchain::Id::Gcc: {
    cppflags.AddPreprocessorFlag("-DRANDOM=1");
    cppflags.AddCppCompileFlag("-Wall");
    cppflags.AddCppCompileFlag("-Werror");
    cppflags.AddLinkFlag("-lm");
    break;
  }
  case base::Toolchain::Id::Msvc: {
    cppflags.AddPreprocessorFlag("/DRANDOM=1");
    cppflags.AddCppCompileFlag("/W4");
    break;
  }
  default:
    break;
  }

  cppflags.Build();
}

static void cflags_build_cb(base::Target &cflags) {
  cflags.AddSource("main.c", "src");

  // Toolchain specific code goes here
  switch (cflags.GetToolchain().GetId()) {
  case base::Toolchain::Id::Gcc: {
    cflags.AddPreprocessorFlag("-DRANDOM=1");
    cflags.AddCCompileFlag("-Wall");
    cflags.AddCCompileFlag("-Werror");
    cflags.AddLinkFlag("-lm");
    break;
  }
  case base::Toolchain::Id::Msvc: {
    cflags.AddPreprocessorFlag("/DRANDOM=1");
    cflags.AddCCompileFlag("/W4");
    break;
  }
  default:
    break;
  }

  cflags.Build();
}
