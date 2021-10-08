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

  // Compile Command
  for (auto &object_rel : current_object_files_) {
    object_rel.second.command = ConstructCompileCommand(object_rel.first);
  }

  // Link Command
  current_target_file_.command = ConstructLinkCommand();

  // Load the serialized file
  (void)loader_.Load();

  // Register the tasks
  CompileTask();
  LinkTask();
}

std::string Target::ConstructLinkCommand() const {
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

// Private

void Target::Lock() { lock_ = true; }

void Target::LockedAfterBuild() const {
  env::assert_fatal(!lock_, "Cannot use this function after Target::Build");
}

void Target::UnlockedAfterBuild() const {
  env::assert_fatal(lock_, "Cannot use this function before Target::Build");
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

void Target::PreLink() {
  for (const auto &user_ld : current_lib_deps_.user) {
    current_lib_deps_.internal.emplace(
        internal::Path::CreateExistingPath(user_ld));
  }

  for (const auto &user_ld : current_link_dependencies_.user) {
    current_link_dependencies_.internal.emplace(
        internal::Path::CreateExistingPath(user_ld));
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

void Target::BuildLink() {
  PreLink();

  RecheckFlags(loader_.GetLoadedLinkFlags(), current_link_flags_);
  RecheckDirs(loader_.GetLoadedLibDirs(), current_lib_dirs_);
  RecheckExternalLib(loader_.GetLoadedExternalLibDeps(),
                     current_external_lib_deps_);
  RecheckPaths(loader_.GetLoadedLinkDependencies(),
               current_link_dependencies_.internal);
  RecheckPaths(loader_.GetLoadedLibDeps(), current_lib_deps_.internal);

  if (dirty_) {
    bool success = Command::Execute(current_target_file_.command);
    env::assert_fatal(success, "Failed to link target");
    Store();
    build_ = true;
  }
}

} // namespace buildcc::base
