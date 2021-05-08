#include "target.h"

#include "internal/util.h"

#include "assert_fatal.h"

#include "fmt/format.h"

namespace buildcc::base {

// Public
void Target::AddSourceAbsolute(const fs::path &absolute_input_filepath,
                               const fs::path &absolute_output_filepath) {
  env::assert_fatal(IsValidSource(absolute_input_filepath),
                    fmt::format("{} does not have a valid source extension",
                                absolute_input_filepath.string()));

  fs::path final_input_path =
      fs::path(absolute_input_filepath).make_preferred();
  fs::path final_output_path =
      fs::path(absolute_output_filepath).make_preferred();

  internal::add_path(final_input_path, current_source_files_);
  current_object_files_[final_input_path.string()] = final_output_path.string();
  fs::create_directories(final_output_path.parent_path());
}

void Target::AddSourceAbsolute(const fs::path &absolute_filepath) {
  const fs::path relative =
      absolute_filepath.lexically_relative(env::get_project_root());
  env::assert_fatal(
      relative.string().find("..") == std::string::npos,
      fmt::format("Out of project root path detected for {} -> {}. Use the "
                  "AddSourceAbsolute(abs_input, abs_output) or "
                  "GlobSourceAbsolute(abs_input, abs_output) API",
                  absolute_filepath.string(), relative.string()));

  fs::path absolute_compiled_source = target_intermediate_dir_ / relative;
  absolute_compiled_source.replace_filename(
      absolute_filepath.filename().string() + ".o");
  AddSourceAbsolute(absolute_filepath, absolute_compiled_source);
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
  env::log_trace(name_, __FUNCTION__);

  fs::path absolute_path = target_root_source_dir_ / relative_to_target_path;
  GlobSourcesAbsolute(absolute_path);
}

void Target::GlobSourcesAbsolute(const fs::path &absolute_input_path,
                                 const fs::path &absolute_output_path) {
  for (const auto &p : fs::directory_iterator(absolute_input_path)) {
    if (IsValidSource(p.path())) {
      fs::path output_p =
          absolute_output_path / (p.path().filename().string() + ".o");
      env::log_trace(name_, fmt::format("Added source {} -> {}",
                                        p.path().string(), output_p.string()));
      AddSourceAbsolute(p.path(), output_p);
    }
  }
}

void Target::GlobSourcesAbsolute(const fs::path &absolute_path) {
  for (const auto &p : fs::directory_iterator(absolute_path)) {
    if (IsValidSource(p.path())) {
      env::log_trace(name_, fmt::format("Added source {}", p.path().string()));
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
      internal::quote(GetCompiledSourcePath(current_source));

  // TODO, Check implementation for GetCompiler
  const std::string compiler = GetCompiler(current_source);

  // TODO, This doesn't look clean
  const auto type = GetFileExtType(current_source);
  const std::string &aggregated_compile_flags =
      type == FileExtType::C     ? aggregated_c_compile_flags_
      : type == FileExtType::Cpp ? aggregated_cpp_compile_flags_
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
