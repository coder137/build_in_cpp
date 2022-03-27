/*
 * Copyright 2021-2022 Niket Naidu. All rights reserved.
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

namespace buildcc::internal {

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

    const auto type =
        target_.toolchain_.GetConfig().GetFileExt(absolute_current_source);
    const std::string selected_aggregated_compile_flags =
        target_.SelectCompileFlags(type).value_or("");
    const std::string selected_compiler =
        fmt::format("{}", fs::path(target_.SelectCompiler(type).value_or("")));
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
// {target_root_dir} => `Project::GetRootDir()` /
// `target_relative_to_root`
// {target_build_dir} => `Project::GetBuildDir()` / `toolchain.GetName()`
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

// TODO, Discuss DifferentOutputFolder API
// {target_root_dir} / random / folder / file.cpp -> {target_build_dir} / random
// / folder / file.cpp.o (SAME)
// {OUT_OF_ROOT_FOLDER} / file.cpp -> {target_build_dir} / {USER_OUTPUT_FOLDER}
// / file.cpp.o
// {OUT_OF_ROOT_FOLDER} / random / folder / file.cpp -> {target_build_dir} /
// {USER_OUTPUT_FOLDER} / random / folder / file.cpp.o
fs::path
CompileObject::ConstructObjectPath(const fs::path &absolute_source_file) const {

  // Compute the relative compiled source path
  // Expects to convert
  // 1. {project_root_dir} / file.cpp -> file.cpp
  // 2. {project_root_dir} / folder / file.cpp -> folder / file.cpp
  fs::path relative =
      absolute_source_file.lexically_relative(target_.GetTargetRootDir());

  // Expects to convert
  // 1. {project_root_dir} / .. / file.cpp -> .. / file.cpp
  // 2. {project_root_dir} / .. / folder / file.cpp -> .. / folder / file.cpp
  // - Check if out of root
  // - Convert .. to __
  // NOTE, Similar to how CMake handles out of root files
  std::string relstr = relative.string();
  if (relstr.find("..") != std::string::npos) {
    env::log_warning(
        __FUNCTION__,
        fmt::format("Out of Root Source detected '{}' -> '{}'. Use "
                    "TargetEnv to supply absolute target root "
                    "path -> absolute target build path. By "
                    "default converts '..' to '__'",
                    absolute_source_file.string(), relstr));
    std::replace(relstr.begin(), relstr.end(), '.', '_');
    // Converts above
    // .. / file.cpp -> __ / file.cpp
    // .. / folder / file.cpp -> __ / folder / file.cpp
    relative = relstr;

    // TODO, path replacement found
    // * API
    // AddSourceAbsolute("BUILDCC_HOME / libs / fmt / build.fmt.cpp",
    //                   {"BUILDCC_HOME / libs / fmt", "fmt"});

    // Converts above
    // .. / file.cpp -> {REPLACEMENT_DIR} / file.cpp
    // .. / folder / file.cpp -> {REPLACEMENT_DIR} / folder / file.cpp
    // relative = relative_replacement_dir / absolute_source_file.filename();

    // std::string absolute_source_file_str =
    //     path_as_string(absolute_source_file);
    // auto iter = absolute_source_file_str.find(replacement_strategy.first);
    // relative = absolute_source_file_str.replace(
    //     iter, replacement_strategy.first.length(),
    //     replacement_strategy.second);
  }

  // Compute relative object path
  fs::path absolute_compiled_source = target_.GetTargetBuildDir() / relative;
  absolute_compiled_source.replace_filename(
      fmt::format("{}{}", absolute_source_file.filename().string(),
                  target_.toolchain_.GetConfig().obj_ext));
  return absolute_compiled_source;
}

void CompileObject::BuildObjectCompile(
    std::vector<internal::Path> &source_files,
    std::vector<internal::Path> &dummy_source_files) {
  PreObjectCompile();

  const auto &serialization = target_.serialization_;
  const auto &load_target_schema = serialization.GetLoad();
  const auto &user_target_schema = target_.user_;

  if (!serialization.IsLoaded()) {
    target_.dirty_ = true;
  } else {
    target_.RecheckFlags(load_target_schema.preprocessor_flags,
                         user_target_schema.preprocessor_flags);
    target_.RecheckFlags(load_target_schema.common_compile_flags,
                         user_target_schema.common_compile_flags);
    target_.RecheckFlags(load_target_schema.pch_object_flags,
                         user_target_schema.pch_object_flags);
    target_.RecheckFlags(load_target_schema.asm_compile_flags,
                         user_target_schema.asm_compile_flags);
    target_.RecheckFlags(load_target_schema.c_compile_flags,
                         user_target_schema.c_compile_flags);
    target_.RecheckFlags(load_target_schema.cpp_compile_flags,
                         user_target_schema.cpp_compile_flags);
    target_.RecheckDirs(load_target_schema.include_dirs,
                        user_target_schema.include_dirs);
    target_.RecheckPaths(load_target_schema.internal_headers,
                         user_target_schema.internal_headers);
    target_.RecheckPaths(load_target_schema.internal_compile_dependencies,
                         user_target_schema.internal_compile_dependencies);
  }

  if (target_.dirty_) {
    CompileSources(source_files);
  } else {
    RecompileSources(source_files, dummy_source_files);
  }
}

void CompileObject::PreObjectCompile() {
  auto &target_user_schema = target_.user_;

  // Convert user_source_files to current_source_files
  target_user_schema.internal_sources =
      internal::path_schema_convert(target_user_schema.sources);

  // Convert user_header_files to current_header_files
  target_user_schema.internal_headers =
      internal::path_schema_convert(target_user_schema.headers);

  // Convert user_compile_dependencies to current_compile_dependencies
  target_user_schema.internal_compile_dependencies =
      internal::path_schema_convert(target_user_schema.compile_dependencies);
}

void CompileObject::CompileSources(std::vector<internal::Path> &source_files) {
  const auto &target_user_schema = target_.user_;
  source_files =
      std::vector<internal::Path>(target_user_schema.internal_sources.begin(),
                                  target_user_schema.internal_sources.end());
}

void CompileObject::RecompileSources(
    std::vector<internal::Path> &source_files,
    std::vector<internal::Path> &dummy_source_files) {
  const auto &serialization = target_.serialization_;
  const auto &user_target_schema = target_.user_;
  const auto &previous_source_files = serialization.GetLoad().internal_sources;

  // * Cannot find previous source in current source files
  const bool is_source_removed =
      std::any_of(previous_source_files.begin(), previous_source_files.end(),
                  [&](const internal::Path &p) {
                    return user_target_schema.internal_sources.find(p) ==
                           user_target_schema.internal_sources.end();
                  });

  if (is_source_removed) {
    target_.dirty_ = true;
    target_.SourceRemoved();
  }

  for (const auto &current_file : user_target_schema.internal_sources) {
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

} // namespace buildcc::internal
