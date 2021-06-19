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

#include "target.h"

#include "internal/util.h"

#include "assert_fatal.h"

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

  // TODO, Optimize these
  aggregated_preprocessor_flags_ =
      internal::aggregate(current_preprocessor_flags_);
  aggregated_c_compile_flags_ = internal::aggregate(current_c_compile_flags_);
  aggregated_cpp_compile_flags_ =
      internal::aggregate(current_cpp_compile_flags_);
  aggregated_link_flags_ = internal::aggregate(current_link_flags_);

  aggregated_lib_deps_ =
      fmt::format("{} {}", internal::aggregate(current_external_lib_deps_),
                  internal::aggregate(current_lib_deps_));

  aggregated_include_dirs_ = internal::aggregate_with_prefix(
      prefix_include_dir_, current_include_dirs_);
  aggregated_lib_dirs_ =
      internal::aggregate_with_prefix(prefix_lib_dir_, current_lib_dirs_);

  const bool is_loaded = loader_.Load();
  // TODO, Add more checks for build files physically present
  // NOTE, This can go into the recompile logic
  if (!is_loaded) {
    BuildCompile();
  } else {
    BuildRecompile();
  }

  dirty_ = false;
}

void Target::BuildCompile() {
  CompileSources();
  LinkTargetTask(true);
  Store();
  first_build_ = true;
}

// * Target rebuild depends on
// TODO, Toolchain name
// DONE, Target preprocessor flags
// DONE, Target compile flags
// DONE, Target link flags
// DONE, Target source files
// DONE, Target include dirs
// DONE, Target library dependencies
// TODO, Target library directories
void Target::BuildRecompile() {

  // * Completely compile sources if any of the following change
  // TODO, Toolchain, ASM, C, C++ compiler related to a particular name
  RecheckFlags(loader_.GetLoadedPreprocessorFlags(),
               current_preprocessor_flags_);
  RecheckFlags(loader_.GetLoadedCCompileFlags(), current_c_compile_flags_);
  RecheckFlags(loader_.GetLoadedCppCompileFlags(), current_cpp_compile_flags_);
  RecheckDirs(loader_.GetLoadedIncludeDirs(), current_include_dirs_);
  RecheckPaths(loader_.GetLoadedHeaders(), current_header_files_);

  // * Compile sources
  if (dirty_) {
    CompileSources();
  } else {
    RecompileSources();
  }

  // * Completely rebuild target / link if any of the following change
  // Target compiled source files either during Compile / Recompile
  // Target library dependencies
  RecheckFlags(loader_.GetLoadedLinkFlags(), current_link_flags_);
  RecheckDirs(loader_.GetLoadedLibDirs(), current_lib_dirs_);
  RecheckPaths(loader_.GetLoadedLibDeps(), current_lib_deps_);
  RecheckExternalLib(loader_.GetLoadedExternalLibDeps(),
                     current_external_lib_deps_);
  // TODO, Verify the `physical` presence of the target if dirty_ == false
  LinkTargetTask(dirty_);
  if (dirty_) {
    Store();
    rebuild_ = true;
  }
}

void Target::LinkTarget() {
  // Add compiled sources
  const std::string aggregated_compiled_sources =
      internal::aggregate(GetCompiledSources());

  const std::string output_target =
      internal::Path::CreateNewPath(GetTargetPath()).GetPathAsString();

  const bool success = command_.ConstructAndExecute(
      Link(), {
                  fmt::arg("output", output_target),
                  fmt::arg("link_flags", aggregated_link_flags_),
                  fmt::arg("compiled_sources", aggregated_compiled_sources),
                  fmt::arg("lib_dirs", aggregated_lib_dirs_),
                  fmt::arg("lib_deps", aggregated_lib_deps_),
                  // Toolchain executables here
                  fmt::arg("asm_compiler", toolchain_.GetAsmCompiler()),
                  fmt::arg("c_compiler", toolchain_.GetCCompiler()),
                  fmt::arg("cpp_compiler", toolchain_.GetCppCompiler()),
                  fmt::arg("archiver", toolchain_.GetArchiver()),
                  fmt::arg("linker", toolchain_.GetLinker()),
              });
  env::assert_fatal(success, fmt::format("Compilation failed for: {}", name_));
}

// TODO, Segregated lib_deps and lib_links
// lib_deps are absolute paths to libraries
// lib_links are ones pointed to by lib_dirs and -l parameters
std::string_view Target::Link() const {
  return "{cpp_compiler} {link_flags} {compiled_sources} -o {output} "
         "{lib_dirs} {lib_deps}";
}

} // namespace buildcc::base
