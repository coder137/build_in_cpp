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

#include "target/target.h"

#include "target/common/util.h"

#include "env/assert_fatal.h"

#include "fmt/format.h"

namespace {

constexpr const char *const kIncludeDirs = "include_dirs";
constexpr const char *const kLibDirs = "lib_dirs";

constexpr const char *const kPreprocessorFlags = "preprocessor_flags";
constexpr const char *const kCommonCompileFlags = "common_compile_flags";
constexpr const char *const kLinkFlags = "link_flags";

constexpr const char *const kPchCompileFlags = "pch_compile_flags";
constexpr const char *const kPchObjectFlags = "pch_object_flags";
constexpr const char *const kPchObjectOutput = "pch_object_output";

constexpr const char *const kAsmCompiler = "asm_compiler";
constexpr const char *const kCCompiler = "c_compiler";
constexpr const char *const kCppCompiler = "cpp_compiler";
constexpr const char *const kArchiver = "archiver";
constexpr const char *const kLinker = "linker";

} // namespace

namespace buildcc {

// * Load
// TODO, Verify things that cannot be changed
// * Compile
// Include directories dependencies
// * Link
// Library dependencies
void Target::Build() {
  env::log_trace(name_, __FUNCTION__);

  state_.ExpectsUnlock();
  state_.SetLock();

  // PCH state
  if (!user_.pchs.empty()) {
    state_.SetPch();
  }

  // Source - Object relation
  // Source state
  for (const auto &abs_source : user_.sources) {
    // Set state
    state_.SetSourceState(toolchain_.GetConfig().GetFileExt(abs_source));

    // Relate input source with output object
    compile_object_.AddObjectData(abs_source);
  }

  // Target default arguments
  command_.AddDefaultArguments({
      {kIncludeDirs,
       internal::aggregate_with_prefix(
           toolchain_.GetConfig().prefix_include_dir, GetIncludeDirs())},
      {kLibDirs, internal::aggregate_with_prefix(
                     toolchain_.GetConfig().prefix_lib_dir, GetLibDirs())},

      {kPreprocessorFlags, internal::aggregate(GetPreprocessorFlags())},
      {kCommonCompileFlags, internal::aggregate(GetCommonCompileFlags())},
      //  TODO, Cache more flags here
      // ASM, C and CPP flags
      {kLinkFlags, internal::aggregate(GetLinkFlags())},

      // Toolchain executables here
      {kAsmCompiler, fmt::format("{}", fs::path(toolchain_.GetAsmCompiler()))},
      {kCCompiler, fmt::format("{}", fs::path(toolchain_.GetCCompiler()))},
      {kCppCompiler, fmt::format("{}", fs::path(toolchain_.GetCppCompiler()))},
      {kArchiver, fmt::format("{}", fs::path(toolchain_.GetArchiver()))},
      {kLinker, fmt::format("{}", fs::path(toolchain_.GetLinker()))},
  });

  // Load the serialized file
  (void)serialization_.LoadFromFile();

  // Target State Tasks
  StartTask();
  EndTask();

  // PCH Compile
  if (state_.ContainsPch()) {
    command_.AddDefaultArguments({
        {kPchCompileFlags, internal::aggregate(GetPchCompileFlags())},
        {kPchObjectFlags, internal::aggregate(GetPchObjectFlags())},
        {kPchObjectOutput, fmt::format("{}", compile_pch_.GetObjectPath())},
    });

    compile_pch_.CacheCompileCommand();
    compile_pch_.Task();
  } else {
    command_.AddDefaultArguments({
        {kPchCompileFlags, ""},
        {kPchObjectFlags, ""},
        {kPchObjectOutput, ""},
    });
  }

  // Compile Command
  compile_object_.CacheCompileCommands();
  compile_object_.Task();

  // Link Command
  link_target_.CacheLinkCommand();
  link_target_.Task();

  // Target dependencies
  TaskDeps();
}

} // namespace buildcc
