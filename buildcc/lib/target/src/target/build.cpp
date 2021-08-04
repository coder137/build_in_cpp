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

  // Taskflow parameters
  UpdateName();
  tf_.name(name_);

  // TODO, Optimize these
  aggregated_c_compile_flags_ = internal::aggregate(current_c_compile_flags_);
  aggregated_cpp_compile_flags_ =
      internal::aggregate(current_cpp_compile_flags_);

  command_.AddDefaultArguments({
      {"include_dirs", internal::aggregate_with_prefix(prefix_include_dir_,
                                                       current_include_dirs_)},
      {"lib_dirs",
       internal::aggregate_with_prefix(prefix_lib_dir_, current_lib_dirs_)},

      {"preprocessor_flags", internal::aggregate(current_preprocessor_flags_)},
      {"link_flags", internal::aggregate(current_link_flags_)},

      // Toolchain executables here
      {"asm_compiler", toolchain_.GetAsmCompiler()},
      {"c_compiler", toolchain_.GetCCompiler()},
      {"cpp_compiler", toolchain_.GetCppCompiler()},
      {"archiver", toolchain_.GetArchiver()},
      {"linker", toolchain_.GetLinker()},
  });

  const bool is_loaded = loader_.Load();
  // TODO, Add more checks for build files physically present
  // NOTE, This can go into the recompile logic
  if (!is_loaded) {
    BuildCompile();
  } else {
    BuildRecompile();
  }
}

void Target::BuildCompile() {
  CompileSources();
  LinkTargetTask(true);
  dirty_ = true;
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
  RecheckExternalLib(loader_.GetLoadedExternalLibDeps(),
                     current_external_lib_deps_);
  // TODO, Verify the `physical` presence of the target if dirty_ == false

  LinkTargetTask(dirty_);
  rebuild_ = dirty_;
}

void Target::LinkTarget() {
  // Add compiled sources
  const std::string aggregated_compiled_sources =
      internal::aggregate(GetCompiledSources());

  const std::string output_target =
      internal::Path::CreateNewPath(GetTargetPath()).GetPathAsString();

  const bool success = command_.ConstructAndExecute(
      Link(),
      {
          {"output", output_target},
          {"compiled_sources", aggregated_compiled_sources},
          {"lib_deps",
           fmt::format("{} {}", internal::aggregate(current_external_lib_deps_),
                       internal::aggregate(current_lib_deps_))},
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
