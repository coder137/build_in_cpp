#include "target.h"

#include "internal/util.h"

#include "assert_fatal.h"

namespace buildcc::base {

// Public
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

// Private

std::vector<std::string> Target::CompileSources() {
  env::log_trace(__FUNCTION__, name_);

  std::vector<std::string> compiled_files;
  for (const auto &file : current_source_files_) {
    const auto &current_source = file.GetPathname();
    const std::string compiled_source = GetCompiledSourceName(current_source);

    CompileSource(current_source);
    compiled_files.push_back(compiled_source);
  }

  return compiled_files;
}

std::vector<std::string> Target::RecompileSources() {
  env::log_trace(__FUNCTION__, name_);

  const auto &previous_source_files = loader_.GetLoadedSources();

  // * Cannot find previous source in current source files
  const bool is_source_removed = internal::is_previous_paths_different(
      previous_source_files, current_source_files_);
  if (is_source_removed) {
    dirty_ = true;
    SourceRemoved();
  }

  std::vector<std::string> compiled_files;
  for (const auto &current_file : current_source_files_) {
    const auto &current_source = current_file.GetPathname();
    const std::string compiled_source = GetCompiledSourceName(current_source);

    // Find current_file in the loaded sources
    auto iter = previous_source_files.find(current_file);

    if (iter == previous_source_files.end()) {
      // *1 New source file added to build
      CompileSource(current_source);
      dirty_ = true;
      SourceAdded();
    } else {
      // *2 Current file is updated
      if (current_file.GetLastWriteTimestamp() >
          iter->GetLastWriteTimestamp()) {
        CompileSource(current_source);
        dirty_ = true;
        SourceUpdated();
      } else {
        // *3 Do nothing
      }
    }
    compiled_files.push_back(compiled_source);
  }

  return compiled_files;
}

void Target::CompileSource(const fs::path &current_source) {
  const std::string output_source = GetCompiledSourceName(current_source);
  const std::string compiler = GetCompiler(current_source);

  const std::string &aggregated_compile_flags =
      compiler == toolchain_.GetCCompiler() ? aggregated_c_compile_flags_
                                            : aggregated_cpp_compile_flags_;

  bool success = internal::command(
      CompileCommand(current_source.string(), output_source, compiler,
                     aggregated_preprocessor_flags_, aggregated_compile_flags,
                     aggregated_include_dirs_));

  env::assert_fatal(success,
                    "Compilation failed for: " + current_source.string());
}

std::vector<std::string>
Target::CompileCommand(const std::string &input_source,
                       const std::string &output_source,
                       const std::string &compiler,
                       const std::string &aggregated_preprocessor_flags,
                       const std::string &aggregated_compile_flags,
                       const std::string &aggregated_include_dirs) {
  return {
      compiler,
      aggregated_preprocessor_flags,
      aggregated_include_dirs,
      aggregated_compile_flags,
      "-o",
      output_source,
      "-c",
      input_source,
  };
}

} // namespace buildcc::base
