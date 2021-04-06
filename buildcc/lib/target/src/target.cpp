#include "base/target.h"

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

// RecompileSources
bool IsOneOrMorePreviousPathDeleted(
    const buildcc::internal::path_unordered_set &previous_path,
    const buildcc::internal::path_unordered_set &current_path) {
  bool one_or_more_previous_source_deleted = false;
  for (const auto &file : previous_path) {
    auto iter = current_path.find(file);
    if (iter == current_path.end()) {
      one_or_more_previous_source_deleted = true;
      break;
    }
  }

  return one_or_more_previous_source_deleted;
}

std::string AggregateIncludeDirs(
    const buildcc::internal::path_unordered_set &include_dirs) {
  std::string idir{""};
  for (const auto &dirs : include_dirs) {
    idir += std::string("-I") + dirs.GetPathname().string() + " ";
  }
  return idir;
}

} // namespace

namespace buildcc::base {

void Target::AddSource(
    const std::string &relative_filename,
    const std::filesystem::path &relative_to_base_relative_path) {
  env::log_trace(__FUNCTION__, name_);

  // Check Source
  fs::path absolute_filepath = target_root_source_dir_ /
                               relative_to_base_relative_path /
                               relative_filename;
  env::assert_fatal(fs::exists(absolute_filepath),
                    absolute_filepath.string() + " not found");

  auto current_file =
      buildcc::internal::Path::CreateExistingPath(absolute_filepath);
  env::assert_fatal(current_source_files_.find(current_file) ==
                        current_source_files_.end(),
                    absolute_filepath.string() + " duplicate found");

  current_source_files_.insert(current_file);
}

void Target::AddSource(const std::string &relative_filename) {
  AddSource(relative_filename, "");
}

void Target::AddIncludeDir(const std::string &relative_include_dir) {
  env::log_trace(__FUNCTION__, name_);

  fs::path absolute_include_dir =
      target_root_source_dir_ / relative_include_dir;

  auto current_dir =
      buildcc::internal::Path::CreateNewPath(absolute_include_dir);
  if (current_include_dirs_.find(current_dir) != current_include_dirs_.end()) {
    return;
  }

  uint64_t max_timestamp_count = 0;
  for (const auto d : fs::directory_iterator(absolute_include_dir)) {
    max_timestamp_count = std::max(
        max_timestamp_count,
        static_cast<uint64_t>(d.last_write_time().time_since_epoch().count()));
  }
  current_dir.SetLastWriteTimestamp(max_timestamp_count);
  current_include_dirs_.insert(current_dir);
}

void Target::AddLibDep(const Target &lib_dep) {
  env::log_trace(__FUNCTION__, name_);

  const fs::path lib_dep_path = lib_dep.GetTargetPath();
  env::assert_fatal(fs::exists(lib_dep_path),
                    lib_dep_path.string() + " not found");

  const auto lib_dep_file = internal::Path::CreateExistingPath(lib_dep_path);
  env::assert_fatal(current_lib_deps_.find(lib_dep_file) ==
                        current_lib_deps_.end(),
                    lib_dep_path.string() + " duplicate found");
  current_lib_deps_.insert(lib_dep_file);
}

// * Load
// TODO, Verify things that cannot be changed
// * Compile
// Include directories dependencies
// * Link
// Library dependencies
void Target::Build() {
  env::log_trace(__FUNCTION__, name_);

  const bool is_loaded = loader_.Load();
  if (!is_loaded) {
    dirty_ = true;
  } else {
    // Compilation depends on include dirs
    RecheckIncludeDirs();
  }

  const auto compiled_sources = BuildSources();

  // Linking depends on library dependencies
  if (!dirty_) {
    RecheckLibDeps();
  }
  if (dirty_) {
    BuildTarget(compiled_sources);
    Store();
  }

  // Reset state variables
  dirty_ = false;
}

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

// Linking
void Target::BuildTarget(const std::vector<std::string> &compiled_sources) {
  env::log_trace(__FUNCTION__, name_);

  // Add compiled sources
  std::string aggregated_compiled_sources =
      internal::aggregate_compiled_sources(compiled_sources);
  std::string aggregated_lib_deps =
      internal::aggregate_lib_deps(current_lib_deps_);

  // TODO, Add compiled libs

  // Final Target
  const fs::path target = GetTargetPath();
  bool success = internal::command({
      // TODO, Improve this logic
      // Select cpp compiler for building target only if there is .cpp file
      // added
      // Else use c compiler
      toolchain_.GetCppCompiler(),
      // TODO, Add Link Flags
      aggregated_compiled_sources,
      "-o",
      target.string(),
      aggregated_lib_deps,
  });
  // TODO, Library dependencies come after

  env::assert_fatal(success, "Compilation failed for: " + GetName());
}

// Compilation
std::vector<std::string> Target::BuildSources() {
  if (dirty_) {
    return CompileSources();
  } else {
    return RecompileSources();
  }
}

std::vector<std::string> Target::CompileSources() {
  env::log_trace(__FUNCTION__, name_);
  const std::string aggregated_include_dirs =
      AggregateIncludeDirs(current_include_dirs_);

  std::vector<std::string> compiled_files;
  for (const auto &file : current_source_files_) {
    const auto &current_source = file.GetPathname();
    const std::string compiled_source = GetCompiledSourceName(current_source);

    CompileSource(current_source, aggregated_include_dirs);
    compiled_files.push_back(compiled_source);
  }

  return compiled_files;
}

std::vector<std::string> Target::RecompileSources() {
  env::log_trace(__FUNCTION__, name_);

  const auto &previous_source_files = loader_.GetLoadedSources();

  // * Cannot find previous source in current source files
  bool is_source_removed = IsOneOrMorePreviousPathDeleted(
      previous_source_files, current_source_files_);
  dirty_ = dirty_ || is_source_removed;

  std::string aggregated_include_dirs =
      AggregateIncludeDirs(current_include_dirs_);

  std::vector<std::string> compiled_files;
  for (const auto &current_file : current_source_files_) {
    const auto &current_source = current_file.GetPathname();
    const std::string compiled_source = GetCompiledSourceName(current_source);

    // Find current_file in the loaded sources
    auto iter = previous_source_files.find(current_file);

    if (iter == previous_source_files.end()) {
      // *1 New source file added to build
      env::log_trace("New source added", name_);
      CompileSource(current_source, aggregated_include_dirs);
      dirty_ = true;
    } else {
      // *2 Current file is updated
      if (current_file.GetLastWriteTimestamp() >
          iter->GetLastWriteTimestamp()) {
        env::log_trace("Current file is newer " +
                           current_file.GetPathname().string(),
                       name_);
        CompileSource(current_source, aggregated_include_dirs);
        dirty_ = true;
      } else {
        // *3 Do nothing
      }
    }
    compiled_files.push_back(compiled_source);
  }

  return compiled_files;
}

void Target::CompileSource(const fs::path &current_source,
                           const std::string &aggregated_include_dirs) {
  const std::string compiled_source = GetCompiledSourceName(current_source);
  const std::string compiler = GetCompiler(current_source);
  bool success = internal::command({
      compiler,
      // TODO, Add Preprocessor Flags
      aggregated_include_dirs,
      // TODO, Add C/Cpp Compile Flags
      "-o",
      compiled_source,
      "-c",
      current_source.string(),
  });
  env::assert_fatal(success,
                    "Compilation failed for: " + current_source.string());
}

// Includes
void Target::RecheckIncludeDirs() {
  env::log_trace(__FUNCTION__, name_);

  const auto &previous_include_dirs = loader_.GetLoadedIncludeDirs();
  // * Cannot find previous include dir in current include dirs
  bool is_dir_removed = IsOneOrMorePreviousPathDeleted(previous_include_dirs,
                                                       current_include_dirs_);
  if (is_dir_removed) {
    env::log_trace("One or more include dir is removed", name_);
    dirty_ = true;
    return;
  }

  for (auto &current_dir : current_include_dirs_) {
    auto iter = previous_include_dirs.find(current_dir);

    if (iter == previous_include_dirs.end()) {
      // * New include dir added
      env::log_trace(
          "New include dir added " + current_dir.GetPathname().string(), name_);
      dirty_ = true;
      break;
    } else {
      // * A file in current dir is updated
      if (current_dir.GetLastWriteTimestamp() > iter->GetLastWriteTimestamp()) {
        env::log_trace("Current dir is newer " +
                           current_dir.GetPathname().string(),
                       name_);
        dirty_ = true;
        break;
      } else {
        // * Do nothing
      }
    }
  }
}

void Target::RecheckLibDeps() {
  env::log_trace(__FUNCTION__, name_);
  const auto &previous_lib_deps = loader_.GetLoadedLibDeps();

  bool is_lib_dep_removed =
      IsOneOrMorePreviousPathDeleted(previous_lib_deps, current_lib_deps_);

  if (is_lib_dep_removed) {
    dirty_ = true;
    return;
  }

  for (auto &current_dep : current_lib_deps_) {
    auto iter = previous_lib_deps.find(current_dep);

    if (iter == previous_lib_deps.end()) {
      // * New lib dep added
      dirty_ = true;
      break;
    } else {
      // * Lib dep has updated
      if (current_dep.GetLastWriteTimestamp() > iter->GetLastWriteTimestamp()) {
        env::log_trace("Current dep is newer " +
                           current_dep.GetPathname().string(),
                       name_);
        dirty_ = true;
        break;
      } else {
        // * Do nothing
      }
    }
  }
}

} // namespace buildcc::base
