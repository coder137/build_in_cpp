#ifndef TARGET_INCLUDE_TARGET_H_
#define TARGET_INCLUDE_TARGET_H_

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

namespace buildcc {

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
      : loader_(name, fs::path(env::get_intermediate_build_dir()) / name),
        name_(name), type_(type), toolchain_(toolchain),
        target_root_source_dir_(env::get_project_root() /
                                target_path_relative_to_root),
        target_intermediate_dir_(fs::path(env::get_intermediate_build_dir()) /
                                 name) {
    Initialize();
  }

  // Builders
  void Build();

  // Setters
  void AddSource(const std::string &relative_filename);
  void AddSource(const std::string &relative_filename,
                 const fs::path &relative_to_target_path);

  void AddIncludeDir(const std::string &relative_include_dir);

  // TODO, Add more setters

  // Getters
  fs::path GetTargetPath() { return target_intermediate_dir_ / name_; }
  fs::path GetBinaryPath() { return loader_.GetBinaryPath(); }

  const std::string &GetName() { return name_; }
  const Toolchain &GetToolchain() { return toolchain_; }
  const fs::path &GetTargetRoot() { return target_root_source_dir_; }
  const fs::path &GetTargetIntermediate() { return target_intermediate_dir_; }

  // TODO, Add more getters

private:
  void Initialize();

  // Compiling
  std::vector<std::string> BuildSources();
  std::vector<std::string> CompileSources();
  std::vector<std::string> RecompileSources();
  virtual void CompileSource(const fs::path &current_source,
                             const std::string &aggregated_include_dirs);

  // Includes
  void RecheckIncludeDirs();

  // Linking
  virtual void BuildTarget(const std::vector<std::string> &compiled_sources);

  // Getters
  std::string GetCompiledSourceName(const fs::path &source);
  std::string GetCompiler(const fs::path &source);

  // Fbs
  bool Store();

private:
  // Constructor defined
  std::string name_;
  Toolchain toolchain_;
  fs::path target_root_source_dir_;
  fs::path target_intermediate_dir_;
  TargetType type_;

  // Internal
  buildcc::internal::path_unordered_set current_source_files_;
  buildcc::internal::path_unordered_set current_include_dirs_;

  // TODO, Add more internal variables

  internal::FbsLoader loader_;
  bool dirty_ = false;
};

} // namespace buildcc

#endif
