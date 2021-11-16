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

#include "env/assert_fatal.h"
#include "env/logging.h"
#include "target/target.h"
#include <algorithm>

namespace {

template <typename T, typename C> void CopyVar(const T &source, const C &cb) {
  for (const auto &s : source) {
    cb(s);
  }
}

} // namespace

namespace buildcc::base {

// PUBLIC

// NOTE, std::move performs a copy when `const Target &`
void Target::Copy(const Target &target,
                  std::initializer_list<CopyOption> options) {
  env::log_trace(__FUNCTION__, "Copy by const ref");
  SpecializedCopy<const Target &>(target, options);
}

// NOTE, std::move performs a move when `Target &&`
void Target::Copy(Target &&target, std::initializer_list<CopyOption> options) {
  env::log_trace(__FUNCTION__, "Copy by move");
  SpecializedCopy<Target &&>(std::move(target), options);
}

// PRIVATE

template <typename T>
void Target::SpecializedCopy(T target,
                             std::initializer_list<CopyOption> options) {
  state_.ExpectsUnlock();
  for (const CopyOption o : options) {
    switch (o) {
    case CopyOption::PreprocessorFlags:
      storer_.current_preprocessor_flags =
          std::move(target.storer_.current_preprocessor_flags);
      break;
    case CopyOption::CommonCompileFlags:
      storer_.current_common_compile_flags =
          std::move(target.storer_.current_common_compile_flags);
      break;
    case CopyOption::PchCompileFlags:
      storer_.current_pch_compile_flags =
          std::move(target.storer_.current_pch_compile_flags);
      break;
    case CopyOption::PchObjectFlags:
      storer_.current_pch_object_flags =
          std::move(target.storer_.current_pch_object_flags);
      break;
    case CopyOption::AsmCompileFlags:
      storer_.current_asm_compile_flags =
          std::move(target.storer_.current_asm_compile_flags);
      break;
    case CopyOption::CCompileFlags:
      storer_.current_c_compile_flags =
          std::move(target.storer_.current_c_compile_flags);
      break;
    case CopyOption::CppCompileFlags:
      storer_.current_cpp_compile_flags =
          std::move(target.storer_.current_cpp_compile_flags);
      break;
    case CopyOption::LinkFlags:
      storer_.current_link_flags = std::move(target.storer_.current_link_flags);
      break;
    case CopyOption::CompileDependencies:
      storer_.current_compile_dependencies.user =
          std::move(target.storer_.current_compile_dependencies.user);
      break;
    case CopyOption::LinkDependencies:
      storer_.current_link_dependencies.user =
          std::move(target.storer_.current_link_dependencies.user);
      break;
    case CopyOption::SourceFiles:
      storer_.current_source_files.user =
          std::move(target.storer_.current_source_files.user);
      break;
    case CopyOption::HeaderFiles:
      storer_.current_header_files.user =
          std::move(target.storer_.current_header_files.user);
      break;
    case CopyOption::PchFiles:
      storer_.current_pch_files.user =
          std::move(target.storer_.current_pch_files.user);
      break;
    case CopyOption::LibDeps:
      storer_.current_lib_deps.user =
          std::move(target.storer_.current_lib_deps.user);
      break;
    case CopyOption::IncludeDirs:
      storer_.current_include_dirs =
          std::move(target.storer_.current_include_dirs);
      break;
    case CopyOption::LibDirs:
      storer_.current_lib_dirs = std::move(target.storer_.current_lib_dirs);
      break;
    case CopyOption::ExternalLibDeps:
      storer_.current_external_lib_deps =
          std::move(target.storer_.current_external_lib_deps);
      break;
    default:
      env::assert_fatal<false>("Invalid Option added");
      break;
    }
  }
}

} // namespace buildcc::base
