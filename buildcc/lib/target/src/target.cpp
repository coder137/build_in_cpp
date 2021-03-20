#include "target.h"

// Internal
#include "internal/assert_fatal.h"
#include "internal/fbs_utils.h"

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
      buildcc::internal::Path::CreateExistingPath(absolute_filepath.string());
  internal::assert_fatal(current_source_files_.find(current_file),
                         current_source_files_.end(),
                         absolute_filepath.string() + " duplicate found");

  current_source_files_.insert(current_file);
}

void Target::AddSource(const std::string &relative_filename) {
  AddSource(relative_filename, "");
}

void Target::Build() {
  env::log_trace(__FUNCTION__, name_);

  std::vector<std::string> compiled_sources;
  if (!loader_.Load()) {
    compiled_sources = CompileSources();
    dirty_ = true;
  } else {
    compiled_sources = RecompileSources();
  }

  if (dirty_) {
    BuildTarget(compiled_sources);
  }

  // Write back
  // TODO, Update this with include directory
  internal::fbs_utils_store_target(name_, target_intermediate_dir_, type_,
                                   toolchain_, current_source_files_,
                                   buildcc::internal::path_unordered_set());
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
  // TODO, AggregateSources
  std::string files = "";
  for (const auto &output_file : compiled_sources) {
    files += " " + output_file;
  }

  // TODO, Aggregate include headers
  // TODO, Add compiled libs

  // Final Target
  const fs::path target = target_intermediate_dir_ / name_;
  bool success = Command({
      toolchain_.GetCppCompiler(),
      "-g",
      "-o",
      target.string(),
      files,
  });
  internal::assert_fatal_true(success, "Compilation failed for: " + name_);
}

void Target::CompileSource(const std::string &source) {
  env::log_trace(__FUNCTION__, name_);

  const std::string compiler = GetCompiler(source);
  const std::string output_filename = GetCompiledSourceName(source);
  bool success = Command({
      compiler,
      "-c",
      source,
      "-o",
      output_filename,
  });
  internal::assert_fatal_true(success, "Compilation failed for: " + source);
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
        env::log_trace("Current file is newer " + current_file.GetPathname(),
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
