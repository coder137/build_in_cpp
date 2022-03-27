#include "buildcc.h"

#include "fmt/format.h"

using namespace buildcc;

constexpr const char *const EXE = "build";

// Function Prototypes
static void clean_cb();
static void cpp_target_cb(BaseTarget &cpptarget,
                          const BaseGenerator &cpp_generator);
static void c_target_cb(BaseTarget &ctarget, const BaseGenerator &c_generator);
static void cpp_generator_cb(BaseGenerator &generator);
static void c_generator_cb(BaseGenerator &generator);

int main(int argc, char **argv) {
  // 1. Get arguments
  Args::Init();
  ArgToolchain arg_gcc;
  ArgToolchain arg_msvc;
  Args::AddToolchain("gcc", "Generic gcc toolchain", arg_gcc);
  Args::AddToolchain("msvc", "Generic msvc toolchain", arg_msvc);
  Args::Parse(argc, argv);

  // 2. Initialize your environment
  Register reg;
  ;

  // 3. Pre-build steps
  reg.Clean(clean_cb);

  // 4. Build steps
  // Explicit toolchain - target pairs
  Toolchain_gcc gcc;
  Toolchain_msvc msvc;

  // CPP
  BaseGenerator cpp_generator("cpp_generator", "");
  reg.Build(cpp_generator_cb, cpp_generator);

  ExecutableTarget_gcc g_cpptarget("cpptarget", gcc, "");
  reg.Build(arg_gcc.state, cpp_target_cb, g_cpptarget, cpp_generator);

  ExecutableTarget_msvc m_cpptarget("cpptarget", msvc, "");
  reg.Build(arg_msvc.state, cpp_target_cb, m_cpptarget, cpp_generator);

  reg.Dep(g_cpptarget, cpp_generator);
  reg.Dep(m_cpptarget, cpp_generator);

  // C
  BaseGenerator c_generator("c_generator", "");
  reg.Build(c_generator_cb, c_generator);

  ExecutableTarget_gcc g_ctarget("ctarget", gcc, "");
  reg.Build(arg_gcc.state, c_target_cb, g_ctarget, c_generator);

  ExecutableTarget_msvc m_ctarget("ctarget", msvc, "");
  reg.Build(arg_msvc.state, c_target_cb, m_ctarget, c_generator);

  reg.Dep(g_ctarget, c_generator);
  reg.Dep(m_ctarget, c_generator);

  // Tests
  reg.Test(arg_gcc.state, "{executable}", g_cpptarget);
  reg.Test(arg_gcc.state, "{executable}", g_ctarget);
  reg.Test(arg_msvc.state, "{executable}", m_cpptarget);
  reg.Test(arg_msvc.state, "{executable}", m_ctarget);

  // 6. Build Target
  reg.RunBuild();

  // 7. Test Target
  reg.RunTest();

  // - Clang Compile Commands
  plugin::ClangCompileCommands(
      {&g_cpptarget, &m_cpptarget, &g_ctarget, &m_ctarget})
      .Generate();

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

static void cpp_target_cb(BaseTarget &cpptarget,
                          const BaseGenerator &cpp_generator) {
  const fs::path main_cpp =
      fs::path(cpp_generator.GetValueByIdentifier("main_cpp"))
          .lexically_relative(Project::GetRootDir());
  cpptarget.AddSource(main_cpp);
  cpptarget.Build();
}

static void c_target_cb(BaseTarget &ctarget, const BaseGenerator &c_generator) {
  const fs::path main_c = fs::path(c_generator.GetValueByIdentifier("main_c"))
                              .lexically_relative(Project::GetRootDir());
  ctarget.AddSource(main_c);
  ctarget.Build();
}

static void cpp_generator_cb(BaseGenerator &generator) {
  generator.AddOutput("{gen_build_dir}/main.cpp", "main_cpp");
  generator.AddCommand("python3 {gen_root_dir}/python/gen.py --source_type cpp "
                       "--destination {main_cpp}");
  generator.Build();
}

static void c_generator_cb(BaseGenerator &generator) {
  generator.AddOutput("{gen_build_dir}/main.c", "main_c");
  generator.AddCommand("python3 {gen_root_dir}/python/gen.py --source_type c "
                       "--destination {main_c}");
  generator.Build();
}
