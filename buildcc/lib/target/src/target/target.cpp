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

SourceType Target::GetSourceType(const fs::path &source) const {
  SourceType type = SourceType::Invalid;
  const auto ext = source.extension();

  if (ext == ".c") {
    type = SourceType::C;
  } else if (ext == ".cpp" || ext == ".cxx" || ext == ".cc") {
    type = SourceType::Cpp;
  } else if (ext == ".s" || ext == ".S" || ext == ".asm") {
    type = SourceType::Asm;
  }

  return type;
}

const std::string &Target::GetCompiler(const fs::path &source) const {
  // .cpp -> GetCppCompiler
  // .c / .asm -> GetCCompiler
  switch (GetSourceType(source)) {
  case SourceType::Asm:
    return toolchain_.GetAsmCompiler();
    break;
  case SourceType::C:
    return toolchain_.GetCCompiler();
    break;
  case SourceType::Cpp:
    break;
  default:
    buildcc::env::assert_fatal(false, "Invalid source " + source.string());
    break;
  }
  return toolchain_.GetCppCompiler();
}

fs::path Target::GetCompiledSourcePath(const fs::path &source) const {
  fs::path absolute_compiled_source =
      target_intermediate_dir_ /
      source.lexically_relative(env::get_project_root());
  absolute_compiled_source.replace_filename(source.filename().string() + ".o");
  absolute_compiled_source.make_preferred();
  return absolute_compiled_source;
}

std::vector<std::string> Target::GetCompiledSources() const {
  std::vector<std::string> compiled_sources;
  for (const auto &current_source : current_source_files_) {
    compiled_sources.push_back(
        GetCompiledSourcePath(current_source.GetPathname()).string());
  }
  return compiled_sources;
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
      PathAdded();
      dirty_ = true;
      break;
    } else {
      // * Path is updated
      if (path.GetLastWriteTimestamp() > iter->GetLastWriteTimestamp()) {
        PathUpdated();
        dirty_ = true;
        break;
      } else {
        // * Do nothing
      }
    }
  }
}

void Target::RecheckDirs(const std::unordered_set<std::string> &previous_dirs,
                         const std::unordered_set<std::string> &current_dirs) {
  if (dirty_) {
    return;
  }

  if (previous_dirs != current_dirs) {
    DirChanged();
    dirty_ = true;
  }
}

void Target::RecheckFlags(
    const std::unordered_set<std::string> &previous_flags,
    const std::unordered_set<std::string> &current_flags) {
  if (dirty_) {
    return;
  }

  if (previous_flags != current_flags) {
    FlagChanged();
    dirty_ = true;
  }
}

} // namespace buildcc::base
