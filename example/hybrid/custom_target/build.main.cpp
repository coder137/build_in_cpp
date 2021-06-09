#include "buildcc.h"

#include "clang_compile_commands.h"

using namespace buildcc;

static void clean_cb();
static void gfoolib_build_cb(base::Target &target);
static void mfoolib_build_cb(base::Target &target);
static void cfoolib_build_cb(base::Target &target);

// * NOTE, This is how we add our custom target
class ExecutableTarget_clang : public base::Target {
public:
  ExecutableTarget_clang(
      const std::string &name, const base::Toolchain &toolchain,
      const std::filesystem::path &target_path_relative_to_root)
      : Target(name, base::TargetType::Executable, toolchain,
               target_path_relative_to_root) {}

private:
private:
  // Compiling
  virtual std::vector<std::string>
  CompileCommand(const std::string &input_source,
                 const std::string &output_source, const std::string &compiler,
                 const std::string &aggregated_preprocessor_flags,
                 const std::string &aggregated_compile_flags,
                 const std::string &aggregated_include_dirs) const override {
    return {
        compiler,
        aggregated_preprocessor_flags,
        aggregated_include_dirs,
        aggregated_compile_flags,
        "-o",
        output_source,
        "-c",
        input_source,
    };
  }

  // Linking
  virtual std::vector<std::string>
  Link(const std::string &output_target,
       const std::string &aggregated_link_flags,
       const std::string &aggregated_compiled_sources,
       const std::string &aggregated_lib_dirs,
       const std::string &aggregated_lib_deps) const override {
    return {
        GetToolchain().GetCppCompiler(),
        aggregated_link_flags,
        aggregated_compiled_sources,
        "-o",
        output_target,
        aggregated_lib_dirs,
        aggregated_lib_deps,
    };
  }
};

int main(int argc, char **argv) {
  // 1. Get arguments
  Args args;
  Args::Toolchain clang_w64_windows_gnu;
  args.AddCustomToolchain("clang_w64_windows_gnu", "MSYS MINGW clang compiler",
                          clang_w64_windows_gnu);
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
  base::Toolchain clang("clang_w64_windows_gnu", "llvm-as", "clang", "clang++",
                        "llvm-ar", "ld");
  // * NOTE, Custom clang target added above
  ExecutableTarget_clang c_foolib("CFoolib.exe", clang, "");
  reg.Build(clang_w64_windows_gnu, c_foolib, cfoolib_build_cb);

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
