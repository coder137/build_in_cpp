#ifndef TARGET_INCLUDE_BASE_TARGET_H_
#define TARGET_INCLUDE_BASE_TARGET_H_

#include <filesystem>
#include <functional>
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

enum class FileExtType {
  Asm,
  C,
  Cpp,
  Header,
  Invalid,
};

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
      : name_(name), type_(type), toolchain_(toolchain),
        target_root_source_dir_(env::get_project_root() /
                                target_path_relative_to_root),
        target_intermediate_dir_(fs::path(env::get_intermediate_build_dir()) /
                                 toolchain.GetName() / name),
        loader_(name, target_intermediate_dir_) {
    Initialize();
  }

  Target(const Target &target) = delete;

  // Builders
  void Build();

  // Setters

  // * Sources
  void AddSourceAbsolute(const fs::path &absolute_filepath);
  // TODO, GlobSourceAbsolute
  void AddSource(const std::string &relative_filename);
  void AddSource(const std::string &relative_filename,
                 const fs::path &relative_to_target_path);
  void GlobSources(const fs::path &relative_to_target_path);

  // * Headers
  void AddHeaderAbsolute(const fs::path &absolute_filepath);
  // TODO, GlobHeaderAbsolute
  void AddHeader(const std::string &relative_filename);
  void AddHeader(const std::string &relative_filename,
                 const fs::path &relative_to_target_path);
  void GlobHeaders(const fs::path &relative_to_target_path);

  // * Include and Lib directory
  // TODO, AddIncludeDirAbsolute
  void AddIncludeDir(const fs::path &relative_include_dir,
                     bool glob_header = false);

  // TODO, Rename this API to AddLibDirAbsolute
  void AddLibDir(const fs::path &absolute_lib_dir);

  // * Libraries
  void AddLibDep(const Target &lib_dep);
  void AddLibDep(const std::string &lib_dep);

  // * Flags
  void AddPreprocessorFlag(const std::string &flag);
  void AddCCompileFlag(const std::string &flag);
  void AddCppCompileFlag(const std::string &flag);
  void AddLinkFlag(const std::string &flag);

  // TODO, Add more setters

  // Getters
  std::vector<std::string> CompileCommand(const fs::path &current_source) const;

  fs::path GetTargetPath() const {
    return (GetTargetIntermediateDir() / GetName()).make_preferred();
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
  const internal::path_unordered_set &GetCurrentHeaderFiles() const {
    return current_header_files_;
  }

  bool FirstBuild() const { return first_build_; }
  bool Rebuild() const { return rebuild_; }

  // TODO, Add more getters

public:
  std::string prefix_include_dir_{"-I"};
  std::string prefix_lib_dir_{"-L"};
  std::unordered_set<std::string> valid_c_ext_{".c"};
  std::unordered_set<std::string> valid_cpp_ext_{".cpp", ".cxx", ".cc"};
  std::unordered_set<std::string> valid_asm_ext_{".s", ".S", ".asm"};
  std::unordered_set<std::string> valid_header_ext_{".h", ".hpp"};

protected:
  // Getters
  FileExtType GetFileExtType(const fs::path &filepath) const;
  bool IsValidSource(const fs::path &sourcepath) const;
  bool IsValidHeader(const fs::path &headerpath) const;

  const std::string &GetCompiler(const fs::path &source) const;

  fs::path GetCompiledSourcePath(const fs::path &source) const;
  std::vector<std::string> GetCompiledSources() const;

private:
  void Initialize();

  // Build
  void BuildCompile();
  void BuildRecompile();

  // Compile
  void CompileSources();
  void RecompileSources();

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

  void RecheckDirs(const std::unordered_set<std::string> &previous_dirs,
                   const std::unordered_set<std::string> &current_dirs);
  void RecheckFlags(const std::unordered_set<std::string> &previous_flags,
                    const std::unordered_set<std::string> &current_flags);
  void RecheckExternalLib(
      const std::unordered_set<std::string> &previous_external_libs,
      const std::unordered_set<std::string> &current_external_libs);

  // Helper function
  void RecheckChanged(const std::unordered_set<std::string> &previous,
                      const std::unordered_set<std::string> &current,
                      std::function<void(void)> callback);

  // Linking
  void BuildTarget();

  // * Virtual
  // PreLink();
  // Link();
  // PostLink();

  // TODO, Add Link library paths
  virtual std::vector<std::string>
  Link(const std::string &output_target,
       const std::string &aggregated_link_flags,
       const std::string &aggregated_compiled_sources,
       const std::string &aggregated_lib_dirs,
       const std::string &aggregated_lib_deps) const;

  // Fbs
  bool Store();

  // Callbacks
  void SourceRemoved();
  void SourceAdded();
  void SourceUpdated();
  void PathRemoved();
  void PathAdded();
  void PathUpdated();

  void DirChanged();
  void FlagChanged();
  void ExternalLibChanged();

private:
  // Constructor defined
  std::string name_;
  TargetType type_;
  const Toolchain &toolchain_;
  fs::path target_root_source_dir_;
  fs::path target_intermediate_dir_;

  // Internal
  internal::path_unordered_set current_source_files_;
  internal::path_unordered_set current_header_files_;
  internal::path_unordered_set current_lib_deps_;

  std::unordered_set<std::string> current_external_lib_deps_;

  std::unordered_set<std::string> current_include_dirs_;
  std::unordered_set<std::string> current_lib_dirs_;

  std::unordered_set<std::string> current_preprocessor_flags_;
  std::unordered_set<std::string> current_c_compile_flags_;
  std::unordered_set<std::string> current_cpp_compile_flags_;
  std::unordered_set<std::string> current_link_flags_;

  // TODO, Make appending to this more efficient
  // TODO, Might not need to be persistent
  std::string aggregated_include_dirs_;
  std::string aggregated_lib_dirs_;
  // NOTE, This contains current_external_lib_deps_ + current_lib_deps_
  std::string aggregated_lib_deps_;

  std::string aggregated_preprocessor_flags_;
  std::string aggregated_c_compile_flags_;
  std::string aggregated_cpp_compile_flags_;
  std::string aggregated_link_flags_;

  // TODO, Add more internal variables

  internal::FbsLoader loader_;
  bool dirty_ = false;

  // Build states
  bool first_build_ = false;
  bool rebuild_ = false;
};

} // namespace buildcc::base

#endif
