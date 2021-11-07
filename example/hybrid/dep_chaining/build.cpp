#include "buildcc.h"

#include "fmt/format.h"

using namespace buildcc;

constexpr const char *const EXE = "build";

// Function Prototypes
static void clean_cb();
static void cpp_target_cb(base::Target &cppflags,
                          const base::Generator &maincpp_generator);
static void cpp_generator_cb(base::Generator &generator);

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

  base::Generator cpp_generator("main_generator", "");
  reg.Build(cpp_generator_cb, cpp_generator);

  ExecutableTarget_gcc g_cpptarget("cppflags", gcc, "");
  reg.Build(arg_gcc.state, cpp_target_cb, g_cpptarget, cpp_generator);

  ExecutableTarget_msvc m_cpptarget("cppflags", msvc, "");
  reg.Build(arg_msvc.state, cpp_target_cb, m_cpptarget, cpp_generator);

  //
  reg.Dep(g_cpptarget, cpp_generator);
  reg.Dep(m_cpptarget, cpp_generator);

  // 6. Build Target
  reg.RunBuild();

  // - Clang Compile Commands
  plugin::ClangCompileCommands({&g_cpptarget, &m_cpptarget}).Generate();

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

static void cpp_target_cb(base::Target &cpp_target,
                          const base::Generator &maincpp_generator) {
  const fs::path main_cpp =
      fs::path(maincpp_generator.GetFileByIdentifier("main_cpp"))
          .lexically_relative(env::get_project_root_dir());
  cpp_target.AddSource(main_cpp);
  cpp_target.Build();
}

static void cpp_generator_cb(base::Generator &generator) {
  generator.AddOutput("{gen_build_dir}/main.cpp", "main_cpp");
  generator.AddCommand("python3 {gen_root_dir}/python/gen.py --source_type cpp "
                       "--destination {main_cpp}");
  generator.Build();
}
