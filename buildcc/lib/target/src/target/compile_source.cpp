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

std::optional<std::string> Target::GetCompiler(FileExtType type) const {
  switch (type) {
  case FileExtType::Asm:
    return toolchain_.GetAsmCompiler();
    break;
  case FileExtType::C:
    return toolchain_.GetCCompiler();
    break;
  case FileExtType::Cpp:
    return toolchain_.GetCppCompiler();
    break;
  default:
    break;
  }
  return {};
}

std::optional<std::string> Target::GetCompiledFlags(FileExtType type) const {
  switch (type) {
  case FileExtType::Asm:
    return internal::aggregate(current_asm_compile_flags_);
    break;
  case FileExtType::C:
    return internal::aggregate(current_c_compile_flags_);
    break;
  case FileExtType::Cpp:
    return internal::aggregate(current_cpp_compile_flags_);
    break;
  default:
    break;
  }
  return {};
}

internal::fs_unordered_set Target::GetCompiledSources() const {
  internal::fs_unordered_set compiled_sources;
  for (const auto &p : current_object_files_) {
    compiled_sources.insert(p.second.output);
  }
  return compiled_sources;
}

const Target::OutputInfo &Target::GetObjectInfo(const fs::path &source) const {
  const auto fiter = current_object_files_.find(source);
  env::assert_fatal(fiter != current_object_files_.end(),
                    fmt::format("{} not found", source.string()));
  return current_object_files_.at(source);
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
  fs::path absolute_compiled_source = target_intermediate_dir_ / relative;
  absolute_compiled_source.replace_filename(fmt::format(
      "{}{}", absolute_source_file.filename().string(), config_.obj_ext));
  return absolute_compiled_source;
}

std::string
Target::ConstructCompileCommand(const fs::path &absolute_current_source) const {
  const std::string output = internal::Path::CreateNewPath(
                                 GetObjectInfo(absolute_current_source).output)
                                 .GetPathAsString();
  const std::string input =
      internal::Path::CreateNewPath(absolute_current_source).GetPathAsString();

  const auto type = GetFileExtType(absolute_current_source);
  const std::string aggregated_compile_flags =
      GetCompiledFlags(type).value_or("");
  const std::string compiler = GetCompiler(type).value_or("");
  return command_.Construct(config_.compile_command,
                            {
                                {"compiler", compiler},
                                {"compile_flags", aggregated_compile_flags},
                                {"output", output},
                                {"input", input},
                            });
}

// Compile APIs
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

void Target::PreCompile() {
  // Convert user_source_files to current_source_files
  for (const auto &user_sf : current_source_files_.user) {
    current_source_files_.internal.emplace(
        buildcc::internal::Path::CreateExistingPath(user_sf));
  }

  // Convert user_header_files to current_header_files
  for (const auto &user_hf : current_header_files_.user) {
    current_header_files_.internal.emplace(
        buildcc::internal::Path::CreateExistingPath(user_hf));
  }

  for (const auto &user_cd : current_compile_dependencies_.user) {
    current_compile_dependencies_.internal.emplace(
        internal::Path::CreateExistingPath(user_cd));
  }
}

void Target::BuildCompile(std::vector<fs::path> &source_files,
                          std::vector<fs::path> &dummy_source_files) {
  PreCompile();

  if (!loader_.IsLoaded()) {
    CompileSources(source_files);
    dirty_ = true;
  } else {
    RecheckFlags(loader_.GetLoadedPreprocessorFlags(),
                 current_preprocessor_flags_);
    RecheckFlags(loader_.GetLoadedCommonCompileFlags(),
                 current_common_compile_flags_);
    RecheckFlags(loader_.GetLoadedAsmCompileFlags(),
                 current_asm_compile_flags_);
    RecheckFlags(loader_.GetLoadedCCompileFlags(), current_c_compile_flags_);
    RecheckFlags(loader_.GetLoadedCppCompileFlags(),
                 current_cpp_compile_flags_);
    RecheckDirs(loader_.GetLoadedIncludeDirs(), current_include_dirs_);
    RecheckPaths(loader_.GetLoadedHeaders(), current_header_files_.internal);
    RecheckPaths(loader_.GetLoadedCompileDependencies(),
                 current_compile_dependencies_.internal);

    if (dirty_) {
      CompileSources(source_files);
    } else {
      RecompileSources(source_files, dummy_source_files);
    }
  }
}

} // namespace buildcc::base
