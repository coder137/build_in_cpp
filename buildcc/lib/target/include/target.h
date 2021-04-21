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

  Target(const Target &target) = delete;

  // Builders
  void Build();

  // Setters
  void AddSource(const std::string &relative_filename);
  void AddSource(const std::string &relative_filename,
                 const fs::path &relative_to_target_path);

  void AddIncludeDir(const std::string &relative_include_dir);

  // TODO, Add fs::path version of the same, can be found using lib_dirs
  void AddLibDep(const Target &lib_dep);

  void AddPreprocessorFlag(const std::string &flag);
  void AddCCompileFlag(const std::string &flag);
  void AddCppCompileFlag(const std::string &flag);
  void AddLinkFlag(const std::string &flag);

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

  const internal::path_unordered_set &GetCurrentSourceFiles() const {
    return current_source_files_;
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
  void SourceRemoved();
  void SourceAdded();
  void SourceUpdated();

  void CompileSource(const fs::path &current_source);

  // * Virtual
  // PreCompile();
  // Compile();
  // PostCompile();
  virtual std::vector<std::string>
  CompileCommand(const std::string &input_source,
                 const std::string &output_source, const std::string &compiler,
                 const std::string &aggregated_preprocessor_flags,
                 const std::string &aggregated_compile_flags,
                 const std::string &aggregated_include_dirs) const;

  // Recompilation checks
  void RecheckPaths(const internal::path_unordered_set &previous_path,
                    const internal::path_unordered_set &current_path);
  void RecheckFlags(const std::unordered_set<std::string> &previous_flags,
                    const std::unordered_set<std::string> &current_flags);

  void PathRemoved();
  void PathAdded();
  void PathUpdated();

  void FlagChanged();

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
       const std::string &aggregated_lib_deps) const;

  // Fbs
  bool Store();

private:
  // Constructor defined
  std::string name_;
  const Toolchain &toolchain_;
  fs::path target_root_source_dir_;
  fs::path target_intermediate_dir_;
  TargetType type_;

  // Internal
  internal::path_unordered_set current_source_files_;
  internal::path_unordered_set current_include_dirs_;
  internal::path_unordered_set current_lib_deps_;

  std::unordered_set<std::string> current_preprocessor_flags_;
  std::unordered_set<std::string> current_c_compile_flags_;
  std::unordered_set<std::string> current_cpp_compile_flags_;
  std::unordered_set<std::string> current_link_flags_;

  // TODO, Make appending to this more efficient
  std::string aggregated_include_dirs_;
  std::string aggregated_lib_deps_;

  std::string aggregated_preprocessor_flags_;
  std::string aggregated_c_compile_flags_;
  std::string aggregated_cpp_compile_flags_;
  std::string aggregated_link_flags_;

  // TODO, Add more internal variables

  internal::FbsLoader loader_;
  bool dirty_ = false;
};

} // namespace buildcc::base

#endif
