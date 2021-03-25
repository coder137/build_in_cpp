#include "target.h"

// Internal
#include "internal/assert_fatal.h"

namespace fs = std::filesystem;

namespace {

// RecompileSources
bool IsOneOrMorePreviousSourceDeleted(
    const buildcc::internal::path_unordered_set &previous_source_files,
    const buildcc::internal::path_unordered_set &current_source_files) {
  bool one_or_more_previous_source_deleted = false;
  for (const auto &file : previous_source_files) {
    auto iter = current_source_files.find(file);
    if (iter == current_source_files.end()) {
      one_or_more_previous_source_deleted = true;
      break;
    }
  }

  return one_or_more_previous_source_deleted;
}

bool Command(const std::vector<std::string> &tokens) {
  std::string command{""};
  for (const auto &t : tokens) {
    command += t + " ";
  }
  buildcc::env::log_debug(command, "system");
  return system(command.c_str()) == 0;
}

std::string AggregateSources(const std::vector<std::string> &compiled_sources) {
  std::string files = "";
  for (const auto &output_file : compiled_sources) {
    files += " " + output_file;
  }
  return files;
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

namespace buildcc {

void Target::AddSource(
    const std::string &relative_filename,
    const std::filesystem::path &relative_to_base_relative_path) {
  env::log_trace(__FUNCTION__, name_);

  // Check Source
  fs::path absolute_filepath = target_root_source_dir_ /
                               relative_to_base_relative_path /
                               relative_filename;
  internal::assert_fatal_true(fs::exists(absolute_filepath),
                              absolute_filepath.string() + " not found");

  auto current_file =
      buildcc::internal::Path::CreateExistingPath(absolute_filepath);
  internal::assert_fatal(current_source_files_.find(current_file),
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

void Target::Build() {
  env::log_trace(__FUNCTION__, name_);

  std::vector<std::string> compiled_sources;

  const bool is_loaded = loader_.Load();
  RecheckIncludeDirs();

  if (!is_loaded || dirty_) {
    compiled_sources = CompileSources();
    dirty_ = true;
  } else {
    compiled_sources = RecompileSources();
  }

  if (dirty_) {
    BuildTarget(compiled_sources);
    Store();
  }
}

// PRIVATE

void Target::Initialize() {
  internal::assert_fatal_true(
      env::is_init(),
      "Environment is not initialized. Use the buildcc::env::init API");
  fs::create_directories(target_intermediate_dir_);
}

void Target::BuildTarget(const std::vector<std::string> &compiled_sources) {
  env::log_trace(__FUNCTION__, name_);

  // Add compiled sources
  std::string aggregated_compiled_sources = AggregateSources(compiled_sources);

  // TODO, Add compiled libs

  // Final Target
  const fs::path target = target_intermediate_dir_ / name_;
  bool success = Command({
      // TODO, Improve this logic
      // Select cpp compiler for building target only if there is .cpp file
      // added
      // Else use c compiler
      toolchain_.GetCppCompiler(),
      "-g",
      "-o",
      target.string(),
      aggregated_compiled_sources,
  });
  internal::assert_fatal_true(success, "Compilation failed for: " + name_);
}

void Target::CompileSource(const fs::path &source) {
  env::log_trace(__FUNCTION__, name_);

  // TODO, These are computationally expensive, Cache them
  const std::string compiler = GetCompiler(source);
  const std::string output_filename = GetCompiledSourceName(source);
  const std::string include_dirs = AggregateIncludeDirs(current_include_dirs_);

  bool success = Command({
      compiler,
      source.string(),
      "-c",
      include_dirs,
      "-o",
      output_filename,
  });
  internal::assert_fatal_true(success,
                              "Compilation failed for: " + source.string());
}

std::vector<std::string> Target::CompileSources() {
  env::log_trace(__FUNCTION__, name_);

  std::vector<std::string> compiled_files;
  for (const auto &file : current_source_files_) {
    std::string compiled_filename = GetCompiledSourceName(file.GetPathname());
    CompileSource(file.GetPathname());
    compiled_files.push_back(compiled_filename);
  }

  return compiled_files;
}

std::vector<std::string> Target::RecompileSources() {
  env::log_trace(__FUNCTION__, name_);

  const auto &previous_source_files = loader_.GetLoadedSources();

  // * Cannot find previous source in current source files
  bool is_source_removed = IsOneOrMorePreviousSourceDeleted(
      previous_source_files, current_source_files_);
  dirty_ = dirty_ || is_source_removed;

  std::vector<std::string> compiled_files;
  for (const auto &current_file : current_source_files_) {
    std::string compiled_filename =
        GetCompiledSourceName(current_file.GetPathname());

    // Find current_file in the loaded sources
    auto iter = previous_source_files.find(current_file);

    if (iter == previous_source_files.end()) {
      // *1 New source file added to build
      env::log_trace("New source added", name_);
      CompileSource(current_file.GetPathname());
      dirty_ = true;
    } else {
      // *2 Current file is updated
      if (current_file.GetLastWriteTimestamp() >
          iter->GetLastWriteTimestamp()) {
        env::log_trace("Current file is newer " +
                           current_file.GetPathname().string(),
                       name_);
        CompileSource(current_file.GetPathname());
        dirty_ = true;
      } else {
        // *3 Do nothing
      }
    }
    compiled_files.push_back(compiled_filename);
  }

  return compiled_files;
}

void Target::RecheckIncludeDirs() {
  env::log_trace(__FUNCTION__, name_);

  const auto &previous_include_dirs = loader_.GetLoadedIncludeDirs();
  // * Cannot find previous include dir in current include dirs
  bool is_dir_removed = IsOneOrMorePreviousSourceDeleted(previous_include_dirs,
                                                         current_include_dirs_);
  dirty_ = dirty_ || is_dir_removed;

  for (auto &current_dir : current_include_dirs_) {
    auto iter = previous_include_dirs.find(current_dir);

    if (iter == previous_include_dirs.end()) {
      // * New include dir added
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

std::string Target::GetCompiledSourceName(const fs::path &source) {
  const auto output_filename =
      target_intermediate_dir_ / (source.filename().string() + ".o");
  return output_filename.string();
}

std::string Target::GetCompiler(const fs::path &source) {
  // .cpp -> GetCppCompiler
  // .c / .asm -> GetCCompiler
  std::string compiler = source.extension() == ".cpp"
                             ? toolchain_.GetCppCompiler()
                             : toolchain_.GetCCompiler();
  return compiler;
}

} // namespace buildcc
