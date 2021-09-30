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
  env::log_trace(name_, __FUNCTION__);

  // Taskflow updates
  tf_.name(fmt::format("[{}] {}", toolchain_.GetName(), name_));

  // TODO, Optimize these
  aggregated_asm_compile_flags_ =
      internal::aggregate(current_asm_compile_flags_);
  aggregated_c_compile_flags_ = internal::aggregate(current_c_compile_flags_);
  aggregated_cpp_compile_flags_ =
      internal::aggregate(current_cpp_compile_flags_);

  command_.AddDefaultArguments({
      {"include_dirs", internal::aggregate_with_prefix(prefix_include_dir_,
                                                       current_include_dirs_)},
      {"lib_dirs",
       internal::aggregate_with_prefix(prefix_lib_dir_, current_lib_dirs_)},

      {"preprocessor_flags", internal::aggregate(current_preprocessor_flags_)},
      {"common_compile_flags",
       internal::aggregate(current_common_compile_flags_)},
      {"link_flags", internal::aggregate(current_link_flags_)},

      // Toolchain executables here
      {"asm_compiler", toolchain_.GetAsmCompiler()},
      {"c_compiler", toolchain_.GetCCompiler()},
      {"cpp_compiler", toolchain_.GetCppCompiler()},
      {"archiver", toolchain_.GetArchiver()},
      {"linker", toolchain_.GetLinker()},
  });

  // Load the serialized file
  (void)loader_.Load();

  // Register the tasks
  CompileTask();
  LinkTask();
}

std::string Target::LinkCommand() const {

  // Add compiled sources
  const std::string aggregated_compiled_sources =
      internal::aggregate(GetCompiledSources());

  const std::string output_target =
      internal::Path::CreateNewPath(GetTargetPath()).GetPathAsString();

  return command_.Construct(
      link_command_,
      {
          {"output", output_target},
          {"compiled_sources", aggregated_compiled_sources},
          {"lib_deps",
           fmt::format("{} {}", internal::aggregate(current_external_lib_deps_),
                       internal::aggregate(current_lib_deps_.user))},
      });
}

//

void Target::ConvertForCompile() {
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

void Target::ConvertForLink() {
  for (const auto &user_ld : current_lib_deps_.user) {
    current_lib_deps_.internal.emplace(
        internal::Path::CreateExistingPath(user_ld));
  }

  for (const auto &user_ld : current_link_dependencies_.user) {
    current_link_dependencies_.internal.emplace(
        internal::Path::CreateExistingPath(user_ld));
  }
}

} // namespace buildcc::base
