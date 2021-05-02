#include "target.h"

// Internal
#include "internal/util.h"

// Env
#include "assert_fatal.h"

// Fmt
#include "fmt/format.h"

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
  const auto ext = source.extension().string();

  if (valid_c_ext_.find(ext) != valid_c_ext_.end()) {
    type = SourceType::C;
  } else if (valid_cpp_ext_.find(ext) != valid_cpp_ext_.end()) {
    type = SourceType::Cpp;
  } else if (valid_asm_ext_.find(ext) != valid_asm_ext_.end()) {
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
    buildcc::env::assert_fatal(
        false, fmt::format("Invalid source {}", source.string()));
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
    compiled_sources.push_back(internal::quote(
        GetCompiledSourcePath(current_source.GetPathname()).string()));
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
  RecheckChanged(previous_dirs, current_dirs,
                 std::bind(&Target::DirChanged, this));
}

void Target::RecheckFlags(
    const std::unordered_set<std::string> &previous_flags,
    const std::unordered_set<std::string> &current_flags) {
  RecheckChanged(previous_flags, current_flags,
                 std::bind(&Target::FlagChanged, this));
}

void Target::RecheckExternalLib(
    const std::unordered_set<std::string> &previous_external_libs,
    const std::unordered_set<std::string> &current_external_libs) {
  RecheckChanged(previous_external_libs, current_external_libs,
                 std::bind(&Target::ExternalLibChanged, this));
}

void Target::RecheckChanged(const std::unordered_set<std::string> &previous,
                            const std::unordered_set<std::string> &current,
                            std::function<void(void)> callback) {
  if (dirty_) {
    return;
  }

  if (previous != current) {
    callback();
    dirty_ = true;
  }
}

} // namespace buildcc::base
