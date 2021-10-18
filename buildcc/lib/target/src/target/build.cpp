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
      {"include_dirs",
       internal::aggregate_with_prefix(config_.prefix_include_dir,
                                       GetCurrentIncludeDirs())},
      {"lib_dirs", internal::aggregate_with_prefix(config_.prefix_lib_dir,
                                                   GetCurrentLibDirs())},

      {"preprocessor_flags",
       internal::aggregate(GetCurrentPreprocessorFlags())},
      {"common_compile_flags",
       internal::aggregate(GetCurrentCommonCompileFlags())},
      {"link_flags", internal::aggregate(GetCurrentLinkFlags())},

      // Toolchain executables here
      {"asm_compiler", toolchain_.GetAsmCompiler()},
      {"c_compiler", toolchain_.GetCCompiler()},
      {"cpp_compiler", toolchain_.GetCppCompiler()},
      {"archiver", toolchain_.GetArchiver()},
      {"linker", toolchain_.GetLinker()},
  });

  // Load the serialized file
  (void)loader_.Load();

  // PCH Compile
  if (!GetCurrentPchFiles().empty()) {
    // TODO, Update .output at Constructor
    pch_file_.command = ConstructPchCompileCommand();
    PchTask();
  }

  // Compile Command
  // Link Command
  for (auto &object_rel : object_files_) {
    object_rel.second.command = ConstructCompileCommand(object_rel.first);
  }
  CompileTask();

  // TODO, Update .output at Constructor
  target_file_.command = ConstructLinkCommand();
  LinkTask();
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
