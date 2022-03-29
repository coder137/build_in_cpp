#include "buildcc.h"

using namespace buildcc;

constexpr const char *const EXE = "build";

// Function Prototypes
static void clean_cb();
static void genericadd_ho_cb(TargetInfo &genericadd_ho);
static void genericadd1_build_cb(BaseTarget &genericadd,
                                 const TargetInfo &genericadd_ho);
static void genericadd2_build_cb(BaseTarget &genericadd,
                                 const TargetInfo &genericadd_ho);

int main(int argc, char **argv) {
  // 1. Get arguments
  ArgToolchain arg_gcc;
  ArgToolchain arg_msvc;
  Args::Init()
      .AddToolchain("gcc", "Generic gcc toolchain", arg_gcc)
      .AddToolchain("msvc", "Generic msvc toolchain", arg_msvc)
      .Parse(argc, argv);

  // 2. Initialize your environment
  Register reg;

  // 3. Pre-build steps
  reg.Clean(clean_cb);

  // 4. Build steps
  // Explicit toolchain - target pairs
  Toolchain_gcc gcc;
  Toolchain_msvc msvc;

  // TargetInfo
  TargetInfo gcc_genericadd_ho(gcc, "files");
  TargetInfo msvc_genericadd_ho(msvc, "files");
  reg.Callback(genericadd_ho_cb, gcc_genericadd_ho);
  reg.Callback(genericadd_ho_cb, msvc_genericadd_ho);

  ExecutableTarget_gcc g_genericadd1("generic_add_1", gcc, "files");
  ExecutableTarget_msvc m_genericadd1("generic_add_1", msvc, "files");

  ExecutableTarget_gcc g_genericadd2("generic_add_2", gcc, "files");
  ExecutableTarget_msvc m_genericadd2("generic_add_2", msvc, "files");

  // Select your builds and tests using the .toml files
  reg.Build(arg_gcc.state, genericadd1_build_cb, g_genericadd1,
            gcc_genericadd_ho);
  reg.Build(arg_gcc.state, genericadd2_build_cb, g_genericadd2,
            gcc_genericadd_ho);

  reg.Build(arg_msvc.state, genericadd1_build_cb, m_genericadd1,
            msvc_genericadd_ho);
  reg.Build(arg_msvc.state, genericadd2_build_cb, m_genericadd2,
            msvc_genericadd_ho);

  // 5. Test steps
  // NOTE, For now they are just dummy callbacks
  reg.Test(arg_gcc.state, "{executable}", g_genericadd1);
  reg.Test(arg_msvc.state, "{executable}", m_genericadd1);

  reg.Test(arg_gcc.state, "{executable}", g_genericadd2);
  reg.Test(arg_msvc.state, "{executable}", m_genericadd2);

  // 6. Build Target
  reg.RunBuild();

  // 7. Test Target
  reg.RunTest();

  // 8. Post Build steps

  // - Clang Compile Commands
  plugin::ClangCompileCommands({&g_genericadd1, &m_genericadd1}).Generate();

  // - Plugin Graph
  std::string output = reg.GetTaskflow().dump();
  const bool saved = env::save_file("graph.dot", output, false);
  env::assert_fatal(saved, "Could not save graph.dot file");

  return 0;
}

static void clean_cb() {
  env::log_info(EXE, fmt::format("Cleaning {}", Project::GetBuildDir()));
  fs::remove_all(Project::GetBuildDir());
}

static void genericadd_ho_cb(TargetInfo &genericadd_ho) {
  genericadd_ho.AddIncludeDir("include", true);
}

static void genericadd1_build_cb(BaseTarget &genericadd,
                                 const TargetInfo &genericadd_ho) {
  genericadd.AddSource("src/main1.cpp");
  genericadd.Copy(genericadd_ho, {
                                     SyncOption::IncludeDirs,
                                     SyncOption::HeaderFiles,
                                 });
  genericadd.Build();
}

static void genericadd2_build_cb(BaseTarget &genericadd,
                                 const TargetInfo &genericadd_ho) {
  genericadd.AddSource("src/main2.cpp");
  genericadd.Copy(genericadd_ho, {
                                     SyncOption::IncludeDirs,
                                     SyncOption::HeaderFiles,
                                 });
  genericadd.Build();
}
