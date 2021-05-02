#include "target.h"

#include "internal/util.h"

#include "assert_fatal.h"

#include "fmt/format.h"

namespace buildcc::base {

// Public
void Target::AddSourceAbsolute(const fs::path &absolute_filepath) {
  internal::add_path(absolute_filepath, current_source_files_);
  fs::create_directories(
      GetCompiledSourcePath(absolute_filepath).parent_path());
}

void Target::AddSource(const std::string &relative_filename,
                       const std::filesystem::path &relative_to_target_path) {
  env::log_trace(name_, __FUNCTION__);
  fs::path absolute_filepath =
      target_root_source_dir_ / relative_to_target_path / relative_filename;
  AddSourceAbsolute(absolute_filepath);
}

void Target::AddSource(const std::string &relative_filename) {
  AddSource(relative_filename, "");
}

void Target::GlobSources(const fs::path &relative_to_target_path) {
  fs::path absolute_filepath =
      target_root_source_dir_ / relative_to_target_path;

  for (const auto &p : fs::directory_iterator(absolute_filepath)) {
    const bool regular_file = p.is_regular_file();
    if (!regular_file) {
      continue;
    }

    const std::string ext = p.path().extension().string();
    const bool asm_match = valid_asm_ext_.count(ext) == 1;
    const bool c_match = valid_c_ext_.count(ext) == 1;
    const bool cpp_match = valid_cpp_ext_.count(ext) == 1;

    // TODO, Add path
    if (asm_match || c_match || cpp_match) {
      AddSourceAbsolute(p.path());
    }
  }
}

// Private

void Target::CompileSources() {
  env::log_trace(name_, __FUNCTION__);

  for (const auto &file : current_source_files_) {
    const auto &current_source = file.GetPathname();
    CompileSource(current_source);
  }
}

void Target::RecompileSources() {
  env::log_trace(name_, __FUNCTION__);

  const auto &previous_source_files = loader_.GetLoadedSources();

  // * Cannot find previous source in current source files
  const bool is_source_removed = internal::is_previous_paths_different(
      previous_source_files, current_source_files_);
  if (is_source_removed) {
    dirty_ = true;
    SourceRemoved();
  }

  for (const auto &current_file : current_source_files_) {
    const auto &current_source = current_file.GetPathname();

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
  }
}

void Target::CompileSource(const fs::path &current_source) {
  const bool success = internal::command(CompileCommand(current_source));
  env::assert_fatal(success, fmt::format("Compilation failed for: {}",
                                         current_source.string()));
}

std::vector<std::string>
Target::CompileCommand(const fs::path &current_source) const {
  const std::string output_source =
      internal::quote(GetCompiledSourcePath(current_source).string());
  const std::string compiler = GetCompiler(current_source);

  const auto type = GetSourceType(current_source);

  // TODO, This doesn't look clean
  const std::string &aggregated_compile_flags =
      type == SourceType::C     ? aggregated_c_compile_flags_
      : type == SourceType::Cpp ? aggregated_cpp_compile_flags_
                                : "";

  const std::string input_source = internal::quote(current_source.string());
  return CompileCommand(input_source, output_source, compiler,
                        aggregated_preprocessor_flags_,
                        aggregated_compile_flags, aggregated_include_dirs_);
}

std::vector<std::string>
Target::CompileCommand(const std::string &input_source,
                       const std::string &output_source,
                       const std::string &compiler,
                       const std::string &aggregated_preprocessor_flags,
                       const std::string &aggregated_compile_flags,
                       const std::string &aggregated_include_dirs) const {
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
