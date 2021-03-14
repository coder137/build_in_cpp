#include "buildcc.h"
#include "fatal_assert.h"
#include "fbs_utils.h"

// Spdlog
#include "spdlog/spdlog.h"

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

} // namespace

namespace buildcc {

void Target::AddSource(
    const std::string &relative_filename,
    const std::filesystem::path &relative_to_base_relative_path) {

  // Check Source
  fs::path absolute_filepath =
      relative_path_ / relative_to_base_relative_path / relative_filename;
  assert_fatal_true(fs::exists(absolute_filepath),
                    absolute_filepath.string() + " not found");

  auto current_file =
      buildcc::internal::Path::CreateExistingPath(absolute_filepath.string());
  assert_fatal(current_source_files_.find(current_file),
               current_source_files_.end(),
               absolute_filepath.string() + " duplicate found");

  current_source_files_.insert(current_file);
}

void Target::AddSource(const std::string &relative_filename) {
  AddSource(relative_filename, "");
}

void Target::Build() {
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
  internal::fbs_utils_store_target(name_, relative_path_, type_, toolchain_,
                                   current_source_files_,
                                   buildcc::internal::path_unordered_set());
}

// PRIVATE

void Target::Initialize() {
  // Set spdlog initialization levels
  spdlog::set_level(spdlog::level::trace);
  spdlog::set_pattern("%^[%l]%$ : %v");
}

void Target::BuildTarget(const std::vector<std::string> &compiled_sources) {
  // Add compiled sources
  std::string files = "";
  for (const auto &output_file : compiled_sources) {
    files += " " + output_file;
  }

  // TODO, Add headers
  // TODO, Add compiled libs

  // Final Target
  auto target = relative_path_ / name_;
  std::string command =
      toolchain_.GetCppCompiler() + " -g -o " + target.string() + files;
  spdlog::debug(command);
  int err = system(command.c_str());
  assert_fatal(err, 0, "Compilation failed for: " + name_);
}

void Target::CompileSource(const std::string &source) {
  fs::path source_path = source;
  std::string compiler = source_path.extension() == ".c"
                             ? toolchain_.GetCCompiler()
                             : toolchain_.GetCppCompiler();

  std::string output_filename = source + ".o";
  std::string command = compiler + " -c " + source + " -o " + output_filename;
  spdlog::debug(command);
  int err = system(command.c_str());
  assert_fatal(err, 0, "Compilation failed for: " + source);
}

std::vector<std::string> Target::CompileSources() {
  spdlog::trace(__FUNCTION__);
  std::vector<std::string> compiled_files;
  for (const auto &file : current_source_files_) {
    std::string compiled_filename = file.GetPathname() + ".o";
    CompileSource(file.GetPathname());
    compiled_files.push_back(compiled_filename);
  }

  return compiled_files;
}

std::vector<std::string> Target::RecompileSources() {
  spdlog::trace(__FUNCTION__);
  const auto &previous_source_files = loader_.GetLoadedSources();

  // * Cannot find previous source in current source files
  bool is_source_removed = IsOneOrMorePreviousSourceDeleted(
      previous_source_files, current_source_files_);
  dirty_ = dirty_ || is_source_removed;

  std::vector<std::string> compiled_files;
  for (const auto &current_file : current_source_files_) {
    std::string compiled_filename = current_file.GetPathname() + ".o";

    // Find current_file in the loaded sources
    auto iter = previous_source_files.find(current_file);

    if (iter == previous_source_files.end()) {
      // *1 New source file added to build
      CompileSource(current_file.GetPathname());
      dirty_ = true;
    } else {
      // *2 Current file is updated
      if (current_file.GetLastWriteTimestamp() >
          iter->GetLastWriteTimestamp()) {
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

} // namespace buildcc
