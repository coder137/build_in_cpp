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

namespace buildcc::base {

internal::fs_unordered_set Target::GetCompiledSources() const {
  internal::fs_unordered_set compiled_sources;
  for (const auto &p : object_files_) {
    compiled_sources.insert(p.second.output);
  }
  return compiled_sources;
}

const Target::OutputInfo &Target::GetObjectInfo(const fs::path &source) const {
  const auto fiter = object_files_.find(source);
  env::assert_fatal(fiter != object_files_.end(),
                    fmt::format("{} not found", source.string()));
  return object_files_.at(source);
}

// Construct APIs
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
                    absolute_source_file.string(), relative.string()));
    std::replace(relstr.begin(), relstr.end(), '.', '_');
    relative = relstr;
  }

  // Compute relative object path
  fs::path absolute_compiled_source = target_build_dir_ / relative;
  absolute_compiled_source.replace_filename(fmt::format(
      "{}{}", absolute_source_file.filename().string(), config_.obj_ext));
  return absolute_compiled_source;
}

// NOTE, Since input has been sanitized, absolute current source is valid
std::string
Target::ConstructCompileCommand(const fs::path &absolute_current_source) const {
  const std::string output = internal::Path::CreateNewPath(
                                 GetObjectInfo(absolute_current_source).output)
                                 .GetPathAsString();
  const std::string input =
      internal::Path::CreateNewPath(absolute_current_source).GetPathAsString();

  const auto type = ext_.GetType(absolute_current_source);
  const std::string selected_aggregated_compile_flags =
      ext_.GetCompileFlags(type).value_or("");
  const std::string selected_compiler = ext_.GetCompiler(type).value_or("");
  return command_.Construct(
      config_.compile_command,
      {
          {"compiler", selected_compiler},
          {"compile_flags", selected_aggregated_compile_flags},
          {"output", output},
          {"input", input},
      });
}

// Compile APIs
void Target::CompileSources(std::vector<fs::path> &source_files) {
  std::transform(storer_.current_source_files.internal.begin(),
                 storer_.current_source_files.internal.end(),
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
                    return storer_.current_source_files.internal.find(p) ==
                           storer_.current_source_files.internal.end();
                  });

  if (is_source_removed) {
    dirty_ = true;
    SourceRemoved();
  }

  for (const auto &current_file : storer_.current_source_files.internal) {
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

void Target::PreObjectCompile() {
  // Convert user_source_files to current_source_files
  storer_.current_source_files.Convert();

  // Convert user_header_files to current_header_files
  storer_.current_header_files.Convert();

  // Convert user_compile_dependencies to current_compile_dependencies
  storer_.current_compile_dependencies.Convert();
}

void Target::BuildObjectCompile(std::vector<fs::path> &source_files,
                                std::vector<fs::path> &dummy_source_files) {
  PreObjectCompile();

  if (!loader_.IsLoaded()) {
    dirty_ = true;
  } else {
    RecheckFlags(loader_.GetLoadedPreprocessorFlags(),
                 GetCurrentPreprocessorFlags());
    RecheckFlags(loader_.GetLoadedCommonCompileFlags(),
                 GetCurrentCommonCompileFlags());
    RecheckFlags(loader_.GetLoadedPchObjectFlags(), GetCurrentPchObjectFlags());
    RecheckFlags(loader_.GetLoadedAsmCompileFlags(),
                 GetCurrentAsmCompileFlags());
    RecheckFlags(loader_.GetLoadedCCompileFlags(), GetCurrentCCompileFlags());
    RecheckFlags(loader_.GetLoadedCppCompileFlags(),
                 GetCurrentCppCompileFlags());
    RecheckDirs(loader_.GetLoadedIncludeDirs(), GetCurrentIncludeDirs());
    RecheckPaths(loader_.GetLoadedHeaders(),
                 storer_.current_header_files.internal);
    RecheckPaths(loader_.GetLoadedCompileDependencies(),
                 storer_.current_compile_dependencies.internal);
  }

  if (dirty_) {
    CompileSources(source_files);
  } else {
    RecompileSources(source_files, dummy_source_files);
  }
}

} // namespace buildcc::base
