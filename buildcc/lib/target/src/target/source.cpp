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
void Target::AddSourceAbsolute(const fs::path &absolute_input_filepath) {
  LockedAfterBuild();
  const auto file_ext_type = ext_.GetType(absolute_input_filepath);
  env::assert_fatal(FileExt::IsValidSource(file_ext_type),
                    fmt::format("{} does not have a valid source extension",
                                absolute_input_filepath));

  const fs::path absolute_source =
      fs::path(absolute_input_filepath).make_preferred();
  storer_.current_source_files.user.insert(absolute_source);
}

void Target::GlobSourcesAbsolute(const fs::path &absolute_input_path) {
  for (const auto &p : fs::directory_iterator(absolute_input_path)) {
    const auto file_ext_type = ext_.GetType(p.path());
    if (FileExt::IsValidSource(file_ext_type)) {
      AddSourceAbsolute(p.path());
    }
  }
}

void Target::AddSource(const fs::path &relative_filename,
                       const std::filesystem::path &relative_to_target_path) {
  env::log_trace(name_, __FUNCTION__);

  // Compute the absolute source path
  fs::path absolute_source =
      GetTargetRootDir() / relative_to_target_path / relative_filename;

  AddSourceAbsolute(absolute_source);
}

void Target::GlobSources(const fs::path &relative_to_target_path) {
  env::log_trace(name_, __FUNCTION__);

  fs::path absolute_input_path = GetTargetRootDir() / relative_to_target_path;

  for (const auto &p : fs::directory_iterator(absolute_input_path)) {
    const auto file_ext_type = ext_.GetType(p.path());
    if (FileExt::IsValidSource(file_ext_type)) {
      AddSourceAbsolute(p.path());
    }
  }
}

fs::path
Target::ConstructObjectPath(const fs::path &absolute_source_file) const {
  // Compute the relative compiled source path
  fs::path relative =
      absolute_source_file.lexically_relative(env::get_project_root_dir());

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
                    absolute_source_file, relative));
    std::replace(relstr.begin(), relstr.end(), '.', '_');
    relative = relstr;
  }

  // Compute relative object path
  fs::path absolute_compiled_source = GetTargetBuildDir() / relative;
  absolute_compiled_source.replace_filename(fmt::format(
      "{}{}", absolute_source_file.filename().string(), config_.obj_ext));
  return absolute_compiled_source;
}

} // namespace buildcc::base
