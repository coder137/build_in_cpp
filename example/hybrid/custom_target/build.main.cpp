#include "buildcc.h"

#include "clang_compile_commands.h"

using namespace buildcc;

static void clean_cb();
static void gfoolib_build_cb(base::Target &target);
static void mfoolib_build_cb(base::Target &target);
static void cfoolib_build_cb(base::Target &target);

// * NOTE, This is how we add our custom target
// Support for any custom toolchain can be added like this
// base::Target provides `CompileCommand` and `Link` overrides
class ExecutableTarget_clang : public base::Target {
public:
  ExecutableTarget_clang(
      const std::string &name, const base::Toolchain &toolchain,
      const std::filesystem::path &target_path_relative_to_root)
      : Target(name, base::TargetType::Executable, toolchain,
               target_path_relative_to_root) {}

private:
private:
  virtual std::string_view CompileCommand() const {
    return "{compiler} {preprocessor_flags} {include_dirs} {compile_flags} -o "
           "{output} -c {input}";
  }
  virtual std::string_view Link() const {
    return "{cpp_compiler} {link_flags} {compiled_sources} -o {output} "
           "{lib_dirs} {lib_deps}";
  }
};

int main(int argc, char **argv) {
  // 1. Get arguments
  Args args;
  Args::Toolchain clang_gnu;
  args.AddCustomToolchain("clang_gnu", "Clang GNU compiler", clang_gnu);
  args.Parse(argc, argv);

  // 2. Initialize your environment
  Register reg(args);
  reg.Env();

  // 3. Pre-build steps
  reg.Clean(clean_cb);

  // 4. Build steps
  Toolchain_gcc gcc;
  Toolchain_msvc msvc;

  ExecutableTarget_gcc g_foolib("GFoolib.exe", gcc, "");
  ExecutableTarget_msvc m_foolib("MFoolib.exe", msvc, "");

  reg.Build(args.GetGccToolchain(), g_foolib, gfoolib_build_cb);
  reg.Build(args.GetMsvcToolchain(), m_foolib, mfoolib_build_cb);

  // * NOTE, This is how we add our custom toolchain
  base::Toolchain clang(base::Toolchain::Id::Clang, "clang_gnu", "llvm-as",
                        "clang", "clang++", "llvm-ar", "ld");
  // * NOTE, Custom clang target added above
  ExecutableTarget_clang c_foolib("CFoolib.exe", clang, "");
  reg.Build(clang_gnu, c_foolib, cfoolib_build_cb);

  // 5.
  reg.RunBuild();

  // 6.
  plugin::ClangCompileCommands({&g_foolib, &m_foolib, &c_foolib}).Generate();

  return 0;
}

static void clean_cb() {
  // TODO,
}

static void gfoolib_build_cb(base::Target &target) {
  target.AddSource("src/foo.cpp");
  target.AddIncludeDir("src", true);
  target.AddSource("main.cpp");
  target.Build();
}

static void mfoolib_build_cb(base::Target &target) {
  target.AddCppCompileFlag("/nologo");
  target.AddCppCompileFlag("/EHsc");
  target.AddLinkFlag("/nologo");
  target.AddSource("src/foo.cpp");
  target.AddIncludeDir("src", true);
  target.AddSource("main.cpp");
  target.Build();
}

static void cfoolib_build_cb(base::Target &target) {
  target.AddSource("src/foo.cpp");
  target.AddIncludeDir("src", true);
  target.AddSource("main.cpp");
  target.Build();
}
