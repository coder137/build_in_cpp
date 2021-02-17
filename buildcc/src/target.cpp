#include "buildcc.h"
#include "fatal_assert.h"
#include "fbs_utils.h"

// Spdlog
#include "spdlog/spdlog.h"

namespace fs = std::filesystem;
namespace fbs = schema::internal;

namespace buildcc {

void Target::AddSource(
    const std::string &relative_filename,
    const std::filesystem::path &relative_to_base_relative_path) {
  // Check Source
  fs::path absolute_filepath =
      relative_path_ / relative_to_base_relative_path / relative_filename;

  assert_fatal_true(fs::exists(absolute_filepath),
                    absolute_filepath.string() + " not found");
  assert_fatal(current_source_files_.find(absolute_filepath.string()),
               current_source_files_.end(),
               absolute_filepath.string() + " duplicate found");

  current_source_files_.insert(absolute_filepath.string());
}

void Target::AddSource(const std::string &relative_filename) {
  AddSource(relative_filename, "");
}

void Target::Build() {
  std::vector<std::string> compiled_sources;
  if (!internal::fbs_utils_fbs_target_exists(*this)) {
    compiled_sources = CompileSources();
    dirty_ = true;
  } else {
    compiled_sources = RecompileSources();
  }

  if (dirty_) {
    BuildTarget(compiled_sources);
    internal::fbs_utils_save_fbs_target(*this);
  }
}

// PRIVATE

void Target::Initialize() {
  // Set spdlog initialization levels
  spdlog::set_level(spdlog::level::trace);
  spdlog::set_pattern("%^[%l]%$ : %v");

  // Check for serialized file
  bool exists = internal::fbs_utils_fbs_target_exists(*this);
  if (!exists) {
    dirty_ = true;
    return;
  }

  // Get target
  fbs::TargetT targetT;
  bool is_loaded = internal::fbs_utils_fbs_load_target(*this, &targetT);
  assert_fatal_true(is_loaded,
                    "Expected to load the '" + name_ + "' serialized file");

  // Update sources
  for (const auto &source_it : targetT.source_files) {
    loaded_source_files_.insert(
        internal::File(source_it->filename, source_it->last_write_timestamp));
  }

  // TODO, Update other deps here
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
  system(command.c_str());
}

std::string Target::CompileSource(const std::string &source) {
  fs::path source_path = source;
  std::string compiler = source_path.extension() == ".c"
                             ? toolchain_.GetCCompiler()
                             : toolchain_.GetCppCompiler();

  std::string output_filename = source + ".o";
  std::string command = compiler + " -c " + source + " -o " + output_filename;
  spdlog::debug(command);
  system(command.c_str());
  return output_filename;
}

std::vector<std::string> Target::CompileSources() {
  spdlog::trace(__FUNCTION__);
  std::vector<std::string> compiled_files;
  for (const auto &filename : current_source_files_) {
    std::string compiled_filename = CompileSource(filename);
    compiled_files.push_back(compiled_filename);
  }

  return compiled_files;
}

std::vector<std::string> Target::RecompileSources() {
  spdlog::trace(__FUNCTION__);
  for (const auto &file : loaded_source_files_) {
    auto iter = current_source_files_.find(file.GetFilename());
    if (iter == current_source_files_.end()) {
      // erase this file from the loaded_source_files;
      loaded_source_files_.erase(file);
    }
  }

  std::vector<std::string> compiled_files;
  for (const auto &filename : current_source_files_) {
    auto current_file = internal::File(filename);
    auto iter = loaded_source_files_.find(current_file);
    std::string compiled_filename;

    // New source file added to build
    if (iter == loaded_source_files_.end()) {
      compiled_filename = CompileSource(filename);
      dirty_ = true;
    } else {
      // Current file is updated
      if (current_file.GetLastWriteTimestamp() >
          iter->GetLastWriteTimestamp()) {
        compiled_filename = CompileSource(filename);
        dirty_ = true;
      } else {
        // Do nothing
        compiled_filename = filename + ".o";
      }
    }
    compiled_files.push_back(compiled_filename);
  }

  return compiled_files;
}

} // namespace buildcc
