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

namespace {

constexpr const char *const kIncludeDirs = "include_dirs";
constexpr const char *const kLibDirs = "lib_dirs";

constexpr const char *const kPreprocessorFlags = "preprocessor_flags";
constexpr const char *const kCommonCompileFlags = "common_compile_flags";
constexpr const char *const kLinkFlags = "link_flags";

constexpr const char *const kPchCompileFlags = "pch_compile_flags";
constexpr const char *const kPchObjectFlags = "pch_object_flags";

constexpr const char *const kAsmCompiler = "asm_compiler";
constexpr const char *const kCCompiler = "c_compiler";
constexpr const char *const kCppCompiler = "cpp_compiler";
constexpr const char *const kArchiver = "archiver";
constexpr const char *const kLinker = "linker";

} // namespace

namespace buildcc::base {

// * Load
// TODO, Verify things that cannot be changed
// * Compile
// Include directories dependencies
// * Link
// Library dependencies
void Target::Build() {
  LockedAfterBuild();
  Lock();

  env::log_trace(name_, __FUNCTION__);

  // Taskflow updates
  tf_.name(fmt::format("[{}] {}", toolchain_.GetName(), name_));

  command_.AddDefaultArguments({
      {kIncludeDirs, internal::aggregate_with_prefix(config_.prefix_include_dir,
                                                     GetCurrentIncludeDirs())},
      {kLibDirs, internal::aggregate_with_prefix(config_.prefix_lib_dir,
                                                 GetCurrentLibDirs())},

      {kPreprocessorFlags, internal::aggregate(GetCurrentPreprocessorFlags())},
      {kCommonCompileFlags,
       internal::aggregate(GetCurrentCommonCompileFlags())},
      {kLinkFlags, internal::aggregate(GetCurrentLinkFlags())},

      // Toolchain executables here
      {kAsmCompiler, toolchain_.GetAsmCompiler()},
      {kCCompiler, toolchain_.GetCCompiler()},
      {kCppCompiler, toolchain_.GetCppCompiler()},
      {kArchiver, toolchain_.GetArchiver()},
      {kLinker, toolchain_.GetLinker()},
  });

  // Load the serialized file
  (void)loader_.Load();

  // PCH Compile
  if (!GetCurrentPchFiles().empty()) {
    command_.AddDefaultArguments({
        {kPchCompileFlags, internal::aggregate(GetCurrentPchCompileFlags())},
        {kPchObjectFlags, internal::aggregate(GetCurrentPchObjectFlags())},
    });

    pch_.CacheCompileCommand();
    pch_.PchTask();
  } else {
    command_.AddDefaultArguments({
        {kPchObjectFlags, ""},
    });
  }

  // Compile Command
  // Link Command
  for (auto &object_rel : object_files_) {
    object_rel.second.command = ConstructCompileCommand(object_rel.first);
  }
  ObjectTask();

  // TODO, Update .output at Constructor
  target_file_.command = ConstructLinkCommand();
  TargetTask();
}

// Private

void Target::Lock() { state_.lock = true; }

void Target::LockedAfterBuild() const {
  env::assert_fatal(!state_.lock,
                    "Cannot use this function after Target::Build");
}

void Target::UnlockedAfterBuild() const {
  env::assert_fatal(state_.lock,
                    "Cannot use this function before Target::Build");
}

} // namespace buildcc::base
