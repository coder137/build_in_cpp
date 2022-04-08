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

  // Generator
  BaseGenerator cpp_generator("cpp_generator", "");
  BaseGenerator c_generator("c_generator", "");
  Reg::Call()
      .Build(cpp_generator_cb, cpp_generator)
      .Build(c_generator_cb, c_generator);

  // Build steps
  // Explicit toolchain - target pairs
  Toolchain_gcc gcc;
  ExecutableTarget_gcc g_cpptarget("cpptarget", gcc, "");
  ExecutableTarget_gcc g_ctarget("ctarget", gcc, "");
  Reg::Toolchain(arg_gcc.state)
      .Build(cpp_target_cb, g_cpptarget, cpp_generator)
      .Build(c_target_cb, g_ctarget, c_generator)
      .Dep(g_cpptarget, cpp_generator)
      .Dep(g_ctarget, c_generator)
      .Test("{executable}", g_cpptarget)
      .Test("{executable}", g_ctarget);

  Toolchain_msvc msvc;
  ExecutableTarget_msvc m_cpptarget("cpptarget", msvc, "");
  ExecutableTarget_msvc m_ctarget("ctarget", msvc, "");
  Reg::Toolchain(arg_msvc.state)
      .Build(cpp_target_cb, m_cpptarget, cpp_generator)
      .Build(c_target_cb, m_ctarget, c_generator)
      .Dep(m_cpptarget, cpp_generator)
      .Dep(m_ctarget, c_generator)
      .Test("{executable}", m_cpptarget)
      .Test("{executable}", m_ctarget);

  // Build and Test
  Reg::Run();

  // - Clang Compile Commands
  plugin::ClangCompileCommands(
      {&g_cpptarget, &m_cpptarget, &g_ctarget, &m_ctarget})
      .Generate();

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
