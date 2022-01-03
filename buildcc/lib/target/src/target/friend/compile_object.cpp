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

#include "target/friend/compile_object.h"

#include "target/target.h"

namespace {

constexpr const char *const kCompiler = "compiler";
constexpr const char *const kCompileFlags = "compile_flags";
constexpr const char *const kOutput = "output";
constexpr const char *const kInput = "input";

} // namespace

namespace buildcc::base {

void CompileObject::AddObjectData(const fs::path &absolute_source_path) {
  const fs::path absolute_object_path =
      ConstructObjectPath(absolute_source_path);
  fs::create_directories(absolute_object_path.parent_path());

  object_files_.emplace(absolute_source_path,
                        ObjectData(absolute_object_path, ""));
}

void CompileObject::CacheCompileCommands() {
  for (auto &object_iter : object_files_) {
    const fs::path &absolute_current_source = object_iter.first;

    const std::string output =
        fmt::format("{}", GetObjectData(absolute_current_source).output);
    const std::string input = fmt::format("{}", absolute_current_source);

    const auto type = target_.config_.GetFileExt(absolute_current_source);
    const std::string selected_aggregated_compile_flags =
        target_.SelectCompileFlags(type).value_or("");
    const std::string selected_compiler =
        target_.SelectCompiler(type).value_or("");
    object_iter.second.command = target_.command_.Construct(
        target_.GetConfig().compile_command,
        {
            {kCompiler, selected_compiler},
            {kCompileFlags, selected_aggregated_compile_flags},
            {kOutput, output},
            {kInput, input},
        });
  }
}

fs_unordered_set CompileObject::GetCompiledSources() const {
  fs_unordered_set compiled_sources;
  for (const auto &p : object_files_) {
    compiled_sources.insert(p.second.output);
  }
  return compiled_sources;
}

const CompileObject::ObjectData &
CompileObject::GetObjectData(const fs::path &absolute_source) const {
  const auto fiter = object_files_.find(absolute_source);
  env::assert_fatal(fiter != object_files_.end(),
                    fmt::format("{} not found", absolute_source));
  return object_files_.at(absolute_source);
}

// PRIVATE

// NOTE: If RELATIVE TargetEnv supplied
// {target_root_dir} => `env::get_project_root_dir()` /
// `target_relative_to_root`
// {target_build_dir} => `env::get_project_build_dir()` / `toolchain.GetName()`
// / `name`

// Scenarios
// - {target_root_dir} / source -> {target_build_dir} / source.compiled
// - {target_root_dir} / folder / source -> {target_build_dir} / folder /
// source.compiled
// - {target_root_dir} / .. / source -> {target_build_dir} / __ /
// source.compiled -> Warning
// Prompt using TargetEnv(abs_root, abs_build)
// - {target_absolute_root_dir} / FOOLIB / source -> {target_absolute_build_dir}
// / FOOLIB / source

// TODO, Discuss PathReplacementStrategy API (Grey areas?)
// NOTE, Replace part of the `relative path` with a path of users choice
// - Add `folder/nested` -> FOLDER_NESTED
// - {target_root_dir} / random / folder / nested / source -> {target_build_dir}
// / random / FOLDER_NESTED / source.compiled
fs::path
CompileObject::ConstructObjectPath(const fs::path &absolute_source_file) const {

  // Compute the relative compiled source path
  fs::path relative =
      absolute_source_file.lexically_relative(target_.GetTargetRootDir());

  // TODO, Add path replacement strategy on relative

  // - Check if out of root
  // - Convert .. to __
  // NOTE, Similar to how CMake handles out of root files
  std::string relstr = relative.string();
  if (relstr.find("..") != std::string::npos) {
    env::log_warning(__FUNCTION__,
                     fmt::format("Out of Root Source detected '{}'. Use "
                                 "TargetEnv to supply absolute target root "
                                 "path -> absolute target build path. By "
                                 "default converts '..' to '__'",
                                 relstr));
    std::replace(relstr.begin(), relstr.end(), '.', '_');
    relative = relstr;
  }

  // Compute relative object path
  fs::path absolute_compiled_source = target_.GetTargetBuildDir() / relative;
  absolute_compiled_source.replace_filename(
      fmt::format("{}{}", absolute_source_file.filename().string(),
                  target_.GetConfig().obj_ext));
  return absolute_compiled_source;
}

void CompileObject::BuildObjectCompile(
    std::vector<internal::Path> &source_files,
    std::vector<internal::Path> &dummy_source_files) {
  PreObjectCompile();

  const auto &loader = target_.loader_;
  const auto &storer = target_.storer_;

  if (!loader.IsLoaded()) {
    target_.dirty_ = true;
  } else {
    target_.RecheckFlags(loader.GetLoadedPreprocessorFlags(),
                         target_.GetCurrentPreprocessorFlags());
    target_.RecheckFlags(loader.GetLoadedCommonCompileFlags(),
                         target_.GetCurrentCommonCompileFlags());
    target_.RecheckFlags(loader.GetLoadedPchObjectFlags(),
                         target_.GetCurrentPchObjectFlags());
    target_.RecheckFlags(loader.GetLoadedAsmCompileFlags(),
                         target_.GetCurrentAsmCompileFlags());
    target_.RecheckFlags(loader.GetLoadedCCompileFlags(),
                         target_.GetCurrentCCompileFlags());
    target_.RecheckFlags(loader.GetLoadedCppCompileFlags(),
                         target_.GetCurrentCppCompileFlags());
    target_.RecheckDirs(loader.GetLoadedIncludeDirs(),
                        target_.GetIncludeDirs());
    target_.RecheckPaths(loader.GetLoadedHeaders(),
                         storer.current_header_files.internal);
    target_.RecheckPaths(loader.GetLoadedCompileDependencies(),
                         storer.current_compile_dependencies.internal);
  }

  if (target_.dirty_) {
    CompileSources(source_files);
  } else {
    RecompileSources(source_files, dummy_source_files);
  }
}

void CompileObject::PreObjectCompile() {
  auto &storer = target_.storer_;

  // Convert user_source_files to current_source_files
  storer.current_source_files.Convert();

  // Convert user_header_files to current_header_files
  storer.current_header_files.Convert();

  // Convert user_compile_dependencies to current_compile_dependencies
  storer.current_compile_dependencies.Convert();
}

void CompileObject::CompileSources(std::vector<internal::Path> &source_files) {
  const auto &storer = target_.storer_;
  source_files =
      std::vector<internal::Path>(storer.current_source_files.internal.begin(),
                                  storer.current_source_files.internal.end());
}

void CompileObject::RecompileSources(
    std::vector<internal::Path> &source_files,
    std::vector<internal::Path> &dummy_source_files) {
  const auto &loader = target_.loader_;
  const auto &storer = target_.storer_;

  const auto &previous_source_files = loader.GetLoadedSources();

  // * Cannot find previous source in current source files
  const bool is_source_removed =
      std::any_of(previous_source_files.begin(), previous_source_files.end(),
                  [&](const internal::Path &p) {
                    return storer.current_source_files.internal.find(p) ==
                           storer.current_source_files.internal.end();
                  });

  if (is_source_removed) {
    target_.dirty_ = true;
    target_.SourceRemoved();
  }

  for (const auto &current_file : storer.current_source_files.internal) {
    // const auto &current_source = current_file.GetPathname();

    // Find current_file in the loaded sources
    auto iter = previous_source_files.find(current_file);

    if (iter == previous_source_files.end()) {
      // *1 New source file added to build
      source_files.push_back(current_file);
      target_.dirty_ = true;
      target_.SourceAdded();
    } else {
      // *2 Current file is updated
      if (current_file.GetLastWriteTimestamp() >
          iter->GetLastWriteTimestamp()) {
        source_files.push_back(current_file);
        target_.dirty_ = true;
        target_.SourceUpdated();
      } else {
        // ELSE
        // *3 Do nothing
        dummy_source_files.push_back(current_file);
      }
    }
  }
}

} // namespace buildcc::base
