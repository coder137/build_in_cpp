#ifndef TARGET_INCLUDE_INTERNAL_FBS_LOADER_H_
#define TARGET_INCLUDE_INTERNAL_FBS_LOADER_H_

#include <filesystem>
#include <string>
#include <unordered_set>

#include "path.h"

namespace fs = std::filesystem;

namespace buildcc::internal {

class FbsLoader {
public:
  FbsLoader(const std::string &name, const fs::path &relative_path)
      : name_(name), relative_path_(relative_path) {
    Initialize();
  }

public:
  bool Load();

  // Getters
  bool IsLoaded() const { return loaded_; }

  const path_unordered_set &GetLoadedSources() const { return loaded_sources_; }
  const path_unordered_set &GetLoadedIncludeDirs() const {
    return loaded_include_dirs_;
  }
  const path_unordered_set &GetLoadedLibDeps() const {
    return loaded_lib_deps_;
  }

  fs::path GetBinaryPath() const { return relative_path_ / (name_ + ".bin"); }

private:
  void Initialize();

private:
  std::string name_;
  fs::path relative_path_;
  bool loaded_ = false;

  path_unordered_set loaded_sources_;
  path_unordered_set loaded_include_dirs_;
  path_unordered_set loaded_lib_deps_;
};

} // namespace buildcc::internal

#endif
