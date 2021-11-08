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

void CompileObject::AddObjectData(const fs::path &absolute_source_path,
                                  const fs::path &absolute_object_path) {
  object_files_.emplace(absolute_source_path,
                        ObjectData(absolute_object_path, ""));
}

void CompileObject::CacheCompileCommands() {
  for (auto &object_iter : object_files_) {
    const fs::path &absolute_current_source = object_iter.first;

    const std::string output =
        fmt::format("{}", GetObjectData(absolute_current_source).output);
    const std::string input = fmt::format("{}", absolute_current_source);

    const auto type = target_.ext_.GetType(absolute_current_source);
    const std::string selected_aggregated_compile_flags =
        target_.ext_.GetCompileFlags(type).value_or("");
    const std::string selected_compiler =
        target_.ext_.GetCompiler(type).value_or("");
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

internal::fs_unordered_set CompileObject::GetCompiledSources() const {
  internal::fs_unordered_set compiled_sources;
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

void CompileObject::BuildObjectCompile(
    std::vector<fs::path> &source_files,
    std::vector<fs::path> &dummy_source_files) {
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
                        target_.GetCurrentIncludeDirs());
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

void CompileObject::CompileSources(std::vector<fs::path> &source_files) {
  const auto &storer = target_.storer_;
  std::transform(storer.current_source_files.internal.begin(),
                 storer.current_source_files.internal.end(),
                 std::back_inserter(source_files),
                 [](const buildcc::internal::Path &p) -> fs::path {
                   return p.GetPathname();
                 });
}

void CompileObject::RecompileSources(
    std::vector<fs::path> &source_files,
    std::vector<fs::path> &dummy_source_files) {
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
    const auto &current_source = current_file.GetPathname();

    // Find current_file in the loaded sources
    auto iter = previous_source_files.find(current_file);

    if (iter == previous_source_files.end()) {
      // *1 New source file added to build
      source_files.push_back(current_source);
      target_.dirty_ = true;
      target_.SourceAdded();
    } else {
      // *2 Current file is updated
      if (current_file.GetLastWriteTimestamp() >
          iter->GetLastWriteTimestamp()) {
        source_files.push_back(current_source);
        target_.dirty_ = true;
        target_.SourceUpdated();
      } else {
        // ELSE
        // *3 Do nothing
        dummy_source_files.push_back(current_source);
      }
    }
  }
}

} // namespace buildcc::base
