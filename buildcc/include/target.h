#ifndef BUILDCC_INCLUDE_TARGET_H_
#define BUILDCC_INCLUDE_TARGET_H_

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

namespace buildcc {

namespace fs = std::filesystem;

class Target {
public:
  explicit Target(const std::string &name, TargetType type,
                  const Toolchain &toolchain,
                  const std::filesystem::path &base_relative_path)
      : loader_(name, base_relative_path), name_(name), type_(type),
        toolchain_(toolchain), relative_path_(base_relative_path) {
    Initialize();
  }

  // Builders
  void Build();

  // Setters
  void AddSource(const std::string &relative_filename);
  void AddSource(const std::string &relative_filename,
                 const std::filesystem::path &relative_to_base_relative_path);

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

private:
  // Constructor defined
  std::string name_;
  Toolchain toolchain_;
  std::filesystem::path relative_path_;
  TargetType type_;

  // Internal
  buildcc::internal::path_unordered_set current_source_files_;

  // TODO, Add more internal variables

  internal::SchemaLoader loader_;
  bool dirty_ = false;
};

} // namespace buildcc

#endif
