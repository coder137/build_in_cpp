/*
 * Copyright 2021 Niket Naidu. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "target/target.h"

#include "target/util.h"

#include "env/assert_fatal.h"

#include "fmt/format.h"

namespace buildcc::base {

// Public
void Target::AddSourceAbsolute(const fs::path &absolute_input_filepath,
                               const fs::path &absolute_output_filepath) {
  env::assert_fatal(IsValidSource(absolute_input_filepath),
                    fmt::format("{} does not have a valid source extension",
                                absolute_input_filepath.string()));

  const fs::path absolute_source =
      fs::path(absolute_input_filepath).make_preferred();
  internal::add_path(absolute_source, current_source_files_);

  // Relate input source files with output object files
  const auto absolute_compiled_source =
      internal::Path::CreateNewPath(absolute_output_filepath);
  fs::create_directories(absolute_compiled_source.GetPathname().parent_path());
  current_object_files_.insert(
      {absolute_source.native(), absolute_compiled_source});
}

void Target::GlobSourcesAbsolute(const fs::path &absolute_input_path,
                                 const fs::path &absolute_output_path) {
  for (const auto &p : fs::directory_iterator(absolute_input_path)) {
    if (IsValidSource(p.path())) {
      fs::path absolute_output_source =
          absolute_output_path /
          fmt::format("{}{}", p.path().filename().string(), obj_ext_);
      AddSourceAbsolute(p.path(), absolute_output_source);
    }
  }
}

void Target::AddSource(const fs::path &relative_filename,
                       const std::filesystem::path &relative_to_target_path) {
  env::log_trace(name_, __FUNCTION__);

  // Compute the absolute source path
  fs::path absolute_source =
      target_root_source_dir_ / relative_to_target_path / relative_filename;
  absolute_source.make_preferred();

  // Compute the relative compiled source path
  fs::path relative =
      absolute_source.lexically_relative(env::get_project_root_dir());

  // - Check if out of root
  // - Convert .. to __
  // NOTE, Similar to how CMake handles out of root files
  std::string relstr = relative.string();
  if (relstr.find("..") != std::string::npos) {
    // TODO, If unnecessary, remove this warning
    env::log_warning(
        name_,
        fmt::format("Out of project root path detected for {} -> "
                    "{}.\nConverting .. to __ but it is recommended to use the "
                    "AddSourceAbsolute(abs_source_input, abs_obj_output) or "
                    "GlobSourceAbsolute(abs_source_input, abs_obj_output) "
                    "API if possible.",
                    absolute_source.string(), relative.string()));
    std::replace(relstr.begin(), relstr.end(), '.', '_');
    relative = relstr;
  }

  // Compute relative object path
  fs::path absolute_compiled_source = target_intermediate_dir_ / relative;
  absolute_compiled_source.replace_filename(
      fmt::format("{}{}", absolute_source.filename().string(), obj_ext_));

  AddSourceAbsolute(absolute_source, absolute_compiled_source);
}

void Target::GlobSources(const fs::path &relative_to_target_path) {
  env::log_trace(name_, __FUNCTION__);

  fs::path absolute_input_path =
      target_root_source_dir_ / relative_to_target_path;

  for (const auto &p : fs::directory_iterator(absolute_input_path)) {
    if (IsValidSource(p.path())) {
      AddSource(p.path().lexically_relative(target_root_source_dir_));
    }
  }
}

// Private

void Target::CompileSources() {
  env::log_trace(name_, __FUNCTION__);
  std::vector<fs::path> compile_sources;
  std::transform(current_source_files_.begin(), current_source_files_.end(),
                 std::back_inserter(compile_sources),
                 [](const buildcc::internal::Path &p) -> fs::path {
                   return p.GetPathname();
                 });
  CompileTargetTask(std::move(compile_sources), std::vector<fs::path>());
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

  std::vector<fs::path> compile_sources;
  std::vector<fs::path> dummy_compile_sources;
  for (const auto &current_file : current_source_files_) {
    const auto &current_source = current_file.GetPathname();

    // Find current_file in the loaded sources
    auto iter = previous_source_files.find(current_file);

    if (iter == previous_source_files.end()) {
      // *1 New source file added to build
      compile_sources.push_back(current_source);
      dirty_ = true;
      SourceAdded();
    } else {
      // *2 Current file is updated
      if (current_file.GetLastWriteTimestamp() >
          iter->GetLastWriteTimestamp()) {
        compile_sources.push_back(current_source);
        dirty_ = true;
        SourceUpdated();
      } else {
        // TODO, Verify the `physical` presence of object file

        // ELSE
        // *3 Do nothing
        dummy_compile_sources.push_back(current_source);
      }
    }
  }

  CompileTargetTask(std::move(compile_sources),
                    std::move(dummy_compile_sources));
}

void Target::CompileSource(const fs::path &current_source) const {
  const bool success =
      internal::Command::Execute(CompileCommand(current_source));
  env::assert_fatal(success, fmt::format("Compilation failed for: {}",
                                         current_source.string()));
}

std::string Target::CompileCommand(const fs::path &current_source) const {
  const std::string output =
      GetCompiledSourcePath(current_source).GetPathAsString();
  const std::string input =
      internal::Path::CreateExistingPath(current_source).GetPathAsString();

  // TODO, Check implementation for GetCompiler
  const std::string compiler = GetCompiler(current_source);

  // TODO, This doesn't look clean
  const auto type = GetFileExtType(current_source);
  const std::string &aggregated_compile_flags =
      type == FileExtType::C     ? aggregated_c_compile_flags_
      : type == FileExtType::Cpp ? aggregated_cpp_compile_flags_
                                 : "";

  return command_.Construct(compile_command_,
                            {
                                {"compiler", compiler},
                                {"compile_flags", aggregated_compile_flags},
                                {"output", output},
                                {"input", input},
                            });
}

} // namespace buildcc::base
