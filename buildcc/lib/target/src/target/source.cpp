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
  current_source_files_.user.insert(absolute_source);

  // Relate input source files with output object files
  const auto absolute_compiled_source =
      fs::path(absolute_output_filepath).make_preferred();
  fs::create_directories(absolute_compiled_source.parent_path());
  current_object_files_.emplace(absolute_source, absolute_compiled_source);
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

void Target::RecompileSources(
    const internal::geninfo_unordered_map &previous_info,
    const internal::geninfo_unordered_map &current_info,
    std::vector<const internal::GenInfo *> &output_generated_files,
    std::vector<const internal::GenInfo *> &output_dummy_generated_files) {
  // RecompileSources
  bool build = false;

  // previous_info has more items than current_info
  for (const auto &pi : previous_info) {
    if (current_info.find(pi.first) == current_info.end()) {
      SourceRemoved();
      build = true;
      break;
    }
  }

  // Check all files individually
  for (const auto &ci : current_info) {
    if (previous_info.find(ci.first) == previous_info.end()) {
      // current_info has more items than
      // previous_info
      SourceAdded();
      dirty_ = true;
    } else {
      const internal::GenInfo &loaded_geninfo = previous_info.at(ci.first);
      BuilderInterface::RecheckPaths(loaded_geninfo.inputs.internal,
                                     ci.second.inputs.internal);
      RecheckChanged(loaded_geninfo.outputs, ci.second.outputs);
      RecheckChanged(loaded_geninfo.commands, ci.second.commands);
      if (dirty_) {
        SourceUpdated();
      }
    }

    if (dirty_) {
      output_generated_files.push_back(&(ci.second));
      build = true;
    } else {
      output_dummy_generated_files.push_back(&(ci.second));
    }
    dirty_ = false;
  }
  dirty_ = build;
}

std::string Target::CompileCommand(const fs::path &current_source) const {
  const std::string output =
      internal::Path::CreateNewPath(GetCompiledSourcePath(current_source))
          .GetPathAsString();
  const std::string input =
      internal::Path::CreateExistingPath(current_source).GetPathAsString();

  const auto type = GetFileExtType(current_source);
  const std::string aggregated_compile_flags =
      GetCompiledFlags(type).value_or("");
  const std::string compiler = GetCompiler(current_source).value_or("");
  return command_.Construct(compile_command_,
                            {
                                {"compiler", compiler},
                                {"compile_flags", aggregated_compile_flags},
                                {"output", output},
                                {"input", input},
                            });
}

} // namespace buildcc::base
