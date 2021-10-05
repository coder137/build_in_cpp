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
  LockedAfterBuild();
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

  AddSourceAbsolute(absolute_source, ConstructObjectPath(absolute_source));
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

void Target::CompileSources(std::vector<fs::path> &source_files) {
  std::transform(current_source_files_.internal.begin(),
                 current_source_files_.internal.end(),
                 std::back_inserter(source_files),
                 [](const buildcc::internal::Path &p) -> fs::path {
                   return p.GetPathname();
                 });
}

void Target::RecompileSources(std::vector<fs::path> &source_files,
                              std::vector<fs::path> &dummy_source_files) {
  env::log_trace(name_, __FUNCTION__);

  const auto &previous_source_files = loader_.GetLoadedSources();

  // * Cannot find previous source in current source files
  const bool is_source_removed =
      std::any_of(previous_source_files.begin(), previous_source_files.end(),
                  [&](const internal::Path &p) {
                    return current_source_files_.internal.find(p) ==
                           current_source_files_.internal.end();
                  });

  if (is_source_removed) {
    dirty_ = true;
    SourceRemoved();
  }

  for (const auto &current_file : current_source_files_.internal) {
    const auto &current_source = current_file.GetPathname();

    // Find current_file in the loaded sources
    auto iter = previous_source_files.find(current_file);

    if (iter == previous_source_files.end()) {
      // *1 New source file added to build
      source_files.push_back(current_source);
      dirty_ = true;
      SourceAdded();
    } else {
      // *2 Current file is updated
      if (current_file.GetLastWriteTimestamp() >
          iter->GetLastWriteTimestamp()) {
        source_files.push_back(current_source);
        dirty_ = true;
        SourceUpdated();
      } else {
        // ELSE
        // *3 Do nothing
        dummy_source_files.push_back(current_source);
      }
    }
  }
}

std::string
Target::CompileCommand(const fs::path &absolute_current_source) const {
  UnlockedAfterBuild();
  const std::string output = internal::Path::CreateNewPath(
                                 GetCompiledSourcePath(absolute_current_source))
                                 .GetPathAsString();
  const std::string input =
      internal::Path::CreateExistingPath(absolute_current_source)
          .GetPathAsString();

  const auto type = GetFileExtType(absolute_current_source);
  const std::string aggregated_compile_flags =
      GetCompiledFlags(type).value_or("");
  const std::string compiler = GetCompiler(type).value_or("");
  return command_.Construct(compile_command_,
                            {
                                {"compiler", compiler},
                                {"compile_flags", aggregated_compile_flags},
                                {"output", output},
                                {"input", input},
                            });
}

} // namespace buildcc::base
