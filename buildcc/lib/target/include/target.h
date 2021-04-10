#ifndef TARGET_INCLUDE_BASE_TARGET_H_
#define TARGET_INCLUDE_BASE_TARGET_H_

#include <filesystem>
#include <string>
#include <unordered_set>
#include <vector>

// Internal
#include "internal/fbs_loader.h"
#include "internal/path.h"

// Buildcc
#include "toolchain.h"

// Env
#include "env.h"

namespace buildcc::base {

namespace fs = std::filesystem;

enum class TargetType {
  Executable,
  StaticLibrary,
  DynamicLibrary,
};

class Target {
public:
  explicit Target(const std::string &name, TargetType type,
                  const Toolchain &toolchain,
                  const fs::path &target_path_relative_to_root)
      : name_(name), toolchain_(toolchain),
        target_root_source_dir_(env::get_project_root() /
                                target_path_relative_to_root),
        target_intermediate_dir_(fs::path(env::get_intermediate_build_dir()) /
                                 name),
        type_(type),
        loader_(name, fs::path(env::get_intermediate_build_dir()) / name) {
    Initialize();
  }

  // Builders
  void Build();

  // Setters
  void AddSource(const std::string &relative_filename);
  void AddSource(const std::string &relative_filename,
                 const fs::path &relative_to_target_path);

  void AddIncludeDir(const std::string &relative_include_dir);

  void AddLibDep(const Target &lib_dep);

  // TODO, Add more setters

  // Getters
  fs::path GetTargetPath() const {
    return (GetTargetIntermediateDir() / GetName());
  }
  fs::path GetBinaryPath() const { return loader_.GetBinaryPath(); }

  // Const references
  const std::string &GetName() const { return name_; }
  const Toolchain &GetToolchain() const { return toolchain_; }
  base::TargetType GetTargetType() const { return type_; }

  const fs::path &GetTargetRootDir() const { return target_root_source_dir_; }
  const fs::path &GetTargetIntermediateDir() const {
    return target_intermediate_dir_;
  }

  // TODO, Add more getters

protected:
  // Getters
  std::string GetCompiledSourceName(const fs::path &source) const;
  std::string GetCompiler(const fs::path &source) const;

private:
  void Initialize();

  // Build
  void BuildCompile();
  void BuildRecompile();

  // Compiling
  std::vector<std::string> CompileSources();
  std::vector<std::string> RecompileSources();
  void CompileSource(const fs::path &current_source,
                     const std::string &aggregated_include_dirs);

  // * Virtual
  // PreCompile();
  // Compile();
  // PostCompile();
  virtual std::vector<std::string>
  CompileCommand(const std::string &input_source,
                 const std::string &output_source, const std::string &compiler,
                 const std::string &aggregated_preprocessor_flags,
                 const std::string &aggregated_compile_flags,
                 const std::string &aggregated_include_dirs);

  // Recompilation checks
  void RecheckIncludeDirs();
  void RecheckLibDeps();

  // Linking
  void BuildTarget(const std::vector<std::string> &compiled_sources);

  // * Virtual
  // PreLink();
  // Link();
  // PostLink();

  // TODO, Add Link library paths
  virtual std::vector<std::string>
  Link(const std::string &output_target,
       const std::string &aggregated_link_flags,
       const std::string &aggregated_compiled_sources,
       const std::string &aggregated_lib_deps);

  // Fbs
  bool Store();

private:
  // Constructor defined
  std::string name_;
  // TODO, Consider making this const reference
  Toolchain toolchain_;
  fs::path target_root_source_dir_;
  fs::path target_intermediate_dir_;
  TargetType type_;

  // Internal
  buildcc::internal::path_unordered_set current_source_files_;
  buildcc::internal::path_unordered_set current_include_dirs_;
  buildcc::internal::path_unordered_set current_lib_deps_;

  // TODO, Add more internal variables

  internal::FbsLoader loader_;
  bool dirty_ = false;
};

} // namespace buildcc::base

#endif
