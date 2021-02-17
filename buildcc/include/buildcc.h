#ifndef BUILDCC_INCLUDE_BUILDCC_H_
#define BUILDCC_INCLUDE_BUILDCC_H_

#include <filesystem>
#include <string>
#include <unordered_set>
#include <vector>

#include "file.h"
#include "toolchain.h"

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
                  const std::filesystem::path &base_relative_path)
      : name_(name), type_(type), toolchain_(toolchain),
        relative_path_(base_relative_path) {
    Initialize();
  }

  // Builders
  void Build();

  // Setters
  void AddSource(const std::string &relative_filename);
  void AddSource(const std::string &relative_filename,
                 const std::filesystem::path &relative_to_base_relative_path);

  // TODO,
  void AppendSources(const std::vector<std::string> &relative_filenames);
  void
  AppendSources(const std::vector<std::string> &relative_filenames,
                const std::filesystem::path &relative_to_base_relative_path);

  void AddIncludeDir(const std::string &dir);
  void AppendIncludeDirs(const std::vector<std::string> &dirs);

  void AddLibDep(const Target &target);
  void AppendLibDeps(const std::vector<Target> &targets);

  void AddLibDir(const std::string &dir);
  void AppendLibDirs(const std::vector<std::string> &dirs);

  // Getters
  const std::string &GetName() const { return name_; }
  const Toolchain &GetToolchain() const { return toolchain_; }
  const std::filesystem::path &GetRelativePath() const {
    return relative_path_;
  }
  TargetType GetTargetType() const { return type_; }

  const std::unordered_set<std::string> &GetSources() const {
    return current_source_files_;
  }

  // TODO, Add more getters

private:
  // Open the serialized file and parse the target
  void Initialize();

  std::string CompileSource(const std::string &source);
  std::vector<std::string> CompileSources();
  std::vector<std::string> RecompileSources();

  void BuildTarget(const std::vector<std::string> &compiled_sources);

private:
  // Constructor defined
  std::string name_;
  Toolchain toolchain_;
  std::filesystem::path relative_path_;
  TargetType type_;

  //
  std::unordered_set<std::string> current_source_files_;
  std::unordered_set<internal::File, internal::FileHash> loaded_source_files_;

  // ! UNUSED
  std::unordered_set<std::string> current_include_dirs_;
  std::unordered_set<std::string> loaded_include_dirs_;

  std::vector<Target> lib_deps_;
  std::vector<std::string> current_lib_dirs;
  std::vector<std::string> loaded_lib_dirs_;

  // Internal
  bool dirty_ = false; // NOTE, Might not be needed
};

} // namespace buildcc

#endif
