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
#include "target_type.h"
#include "toolchain.h"

// Env
#include "env.h"

namespace buildcc {

namespace fs = std::filesystem;

class Target {
public:
  explicit Target(const std::string &name, TargetType type,
                  const Toolchain &toolchain,
                  const std::filesystem::path &target_path_relative_to_root)
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
                 const std::filesystem::path &relative_to_target_path);

  // TODO, Add more setters

  // Getters

  // TODO, Add more getters

private:
  // Open the serialized file and parse the target
  void Initialize();

  std::vector<std::string> CompileSources();
  std::vector<std::string> RecompileSources();

  void BuildTarget(const std::vector<std::string> &compiled_sources);
  void CompileSource(const std::string &source);
  std::string GetCompiledSourceName(const fs::path &source);

private:
  // Constructor defined
  std::string name_;
  Toolchain toolchain_;
  std::filesystem::path target_root_source_dir_;
  std::filesystem::path target_intermediate_dir_;
  TargetType type_;

  // Internal
  buildcc::internal::path_unordered_set current_source_files_;

  // TODO, Add more internal variables

  internal::FbsLoader loader_;
  bool dirty_ = false;
};

} // namespace buildcc

#endif
