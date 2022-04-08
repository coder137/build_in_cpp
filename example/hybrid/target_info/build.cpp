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
  TargetInfo gcc_genericadd_ho(gcc, "files");
  ExecutableTarget_gcc g_genericadd1("generic_add_1", gcc, "files");
  ExecutableTarget_gcc g_genericadd2("generic_add_2", gcc, "files");
  Reg::Toolchain(arg_gcc.state)
      .Func([&]() { gcc.Verify(); })
      .Func(genericadd_ho_cb, gcc_genericadd_ho)
      .Build(genericadd1_build_cb, g_genericadd1, gcc_genericadd_ho)
      .Build(genericadd2_build_cb, g_genericadd2, gcc_genericadd_ho)
      .Test("{executable}", g_genericadd1)
      .Test("{executable}", g_genericadd2);

  Toolchain_msvc msvc;
  TargetInfo msvc_genericadd_ho(msvc, "files");
  ExecutableTarget_msvc m_genericadd1("generic_add_1", msvc, "files");
  ExecutableTarget_msvc m_genericadd2("generic_add_2", msvc, "files");
  Reg::Toolchain(arg_msvc.state)
      .Func([&]() { msvc.Verify(); })
      .Func(genericadd_ho_cb, msvc_genericadd_ho)
      .Build(genericadd1_build_cb, m_genericadd1, msvc_genericadd_ho)
      .Build(genericadd2_build_cb, m_genericadd2, msvc_genericadd_ho)
      .Test("{executable}", m_genericadd1)
      .Test("{executable}", m_genericadd2);

  // Run
  Reg::Run();

  // Post Build steps
  // - Clang Compile Commands
  plugin::ClangCompileCommands({&g_genericadd1, &m_genericadd1}).Generate();

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
