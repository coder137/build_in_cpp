#ifndef BUILDCC_SCHEMA_LOADER_H_
#define BUILDCC_SCHEMA_LOADER_H_

#include <filesystem>
#include <string>
#include <unordered_set>

#include "path.h"

namespace fs = std::filesystem;

namespace buildcc::internal {

class SchemaLoader {
public:
  SchemaLoader(const std::string &name, const fs::path &relative_path)
      : name_(name), relative_path_(relative_path) {
    Initialize();
  }

public:
  bool Load();

  // Getters
  const std::unordered_set<Path, PathHash> &GetLoadedSources() const {
    return loaded_sources_;
  }

private:
  void Initialize();

private:
  std::string name_;
  fs::path relative_path_;

  std::unordered_set<Path, PathHash> loaded_sources_;
};

} // namespace buildcc::internal

#endif
