#include "buildcc.h"

#include "fmt/format.h"

using namespace buildcc;

constexpr const char *const EXE = "build";

// Function Prototypes
static void clean_cb();
static void cpp_target_cb(base::Target &cpptarget,
                          const base::Generator &cpp_generator);
static void c_target_cb(base::Target &ctarget,
                        const base::Generator &c_generator);
static void cpp_generator_cb(base::Generator &generator);
static void c_generator_cb(base::Generator &generator);

int main(int argc, char **argv) {
  // 1. Get arguments
  Args args;
  Args::ToolchainArg arg_gcc;
  Args::ToolchainArg arg_msvc;
  args.AddToolchain("gcc", "Generic gcc toolchain", arg_gcc);
  args.AddToolchain("msvc", "Generic msvc toolchain", arg_msvc);
  args.Parse(argc, argv);

  // 2. Initialize your environment
  Register reg(args);

  // 3. Pre-build steps
  reg.Clean(clean_cb);

  // 4. Build steps
  // Explicit toolchain - target pairs
  Toolchain_gcc gcc;
  Toolchain_msvc msvc;

  // CPP
  base::Generator cpp_generator("cpp_generator", "");
  reg.Build(cpp_generator_cb, cpp_generator);

  ExecutableTarget_gcc g_cpptarget("cpptarget", gcc, "");
  reg.Build(arg_gcc.state, cpp_target_cb, g_cpptarget, cpp_generator);

  ExecutableTarget_msvc m_cpptarget("cpptarget", msvc, "");
  reg.Build(arg_msvc.state, cpp_target_cb, m_cpptarget, cpp_generator);

  reg.Dep(g_cpptarget, cpp_generator);
  reg.Dep(m_cpptarget, cpp_generator);

  // C
  base::Generator c_generator("c_generator", "");
  reg.Build(c_generator_cb, c_generator);

  ExecutableTarget_gcc g_ctarget("ctarget", gcc, "");
  reg.Build(arg_gcc.state, c_target_cb, g_ctarget, c_generator);

  ExecutableTarget_msvc m_ctarget("ctarget", msvc, "");
  reg.Build(arg_msvc.state, c_target_cb, m_ctarget, c_generator);

  reg.Dep(g_ctarget, c_generator);
  reg.Dep(m_ctarget, c_generator);

  // 6. Build Target
  reg.RunBuild();

  // - Clang Compile Commands
  plugin::ClangCompileCommands(
      {&g_cpptarget, &m_cpptarget, &g_ctarget, &m_ctarget})
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

static void cpp_target_cb(base::Target &cpptarget,
                          const base::Generator &cpp_generator) {
  const fs::path main_cpp =
      fs::path(cpp_generator.GetFileByIdentifier("main_cpp"))
          .lexically_relative(env::get_project_root_dir());
  cpptarget.AddSource(main_cpp);
  cpptarget.Build();
}

static void c_target_cb(base::Target &ctarget,
                        const base::Generator &c_generator) {
  const fs::path main_c = fs::path(c_generator.GetFileByIdentifier("main_c"))
                              .lexically_relative(env::get_project_root_dir());
  ctarget.AddSource(main_c);
  ctarget.Build();
}

static void cpp_generator_cb(base::Generator &generator) {
  generator.AddOutput("{gen_build_dir}/main.cpp", "main_cpp");
  generator.AddCommand("python3 {gen_root_dir}/python/gen.py --source_type cpp "
                       "--destination {main_cpp}");
  generator.Build();
}

static void c_generator_cb(base::Generator &generator) {
  generator.AddOutput("{gen_build_dir}/main.c", "main_c");
  generator.AddCommand("python3 {gen_root_dir}/python/gen.py --source_type c "
                       "--destination {main_c}");
  generator.Build();
}
