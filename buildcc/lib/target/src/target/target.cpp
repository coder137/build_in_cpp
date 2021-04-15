#include "target.h"

// Internal
#include "internal/util.h"

// Env
#include "assert_fatal.h"

namespace fs = std::filesystem;

namespace {

bool IsValidTargetType(buildcc::base::TargetType type) {
  switch (type) {
  case buildcc::base::TargetType::Executable:
  case buildcc::base::TargetType::StaticLibrary:
  case buildcc::base::TargetType::DynamicLibrary:
    return true;
    break;
  default:
    return false;
    break;
  }
}

} // namespace

namespace buildcc::base {

// PROTECTED

// Getters
std::string Target::GetCompiledSourceName(const fs::path &source) const {
  const auto output_filename =
      GetTargetIntermediateDir() / (source.filename().string() + ".o");
  return output_filename.string();
}

std::string Target::GetCompiler(const fs::path &source) const {
  // .cpp -> GetCppCompiler
  // .c / .asm -> GetCCompiler
  std::string compiler = source.extension() == ".cpp"
                             ? toolchain_.GetCppCompiler()
                             : toolchain_.GetCCompiler();
  return compiler;
}

// PRIVATE

void Target::Initialize() {
  env::assert_fatal(
      env::is_init(),
      "Environment is not initialized. Use the buildcc::env::init API");
  env::assert_fatal(IsValidTargetType(type_), "Invalid Target Type");
  fs::create_directories(target_intermediate_dir_);
}

// Rechecks
void Target::RecheckPaths(const internal::path_unordered_set &previous_path,
                          const internal::path_unordered_set &current_path) {
  // * Compile sources / Target already requires rebuild
  if (dirty_) {
    return;
  }

  // * Old path is removed
  const bool removed =
      internal::is_previous_paths_different(previous_path, current_path);
  if (removed) {
    PathRemoved();
    dirty_ = true;
    return;
  }

  for (auto &path : current_path) {
    auto iter = previous_path.find(path);

    if (iter == previous_path.end()) {
      // * New path added
      dirty_ = true;
      PathAdded();
      break;
    } else {
      // * Path is updated
      if (path.GetLastWriteTimestamp() > iter->GetLastWriteTimestamp()) {
        dirty_ = true;
        PathUpdated();
        break;
      } else {
        // * Do nothing
      }
    }
  }
}

void Target::RecheckFlags(
    const std::unordered_set<std::string> &previous_flags,
    const std::unordered_set<std::string> &current_flags) {
  if (dirty_) {
    return;
  }

  if (previous_flags != current_flags) {
    dirty_ = true;
  }
}

} // namespace buildcc::base
