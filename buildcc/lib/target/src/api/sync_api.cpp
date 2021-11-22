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

#include "target/api/sync_api.h"

#include "target/target_info.h"

namespace buildcc::base {

template <typename T>
void CopyApi<T>::Copy(const T &target,
                      std::initializer_list<CopyOption> options) {
  env::log_trace(__FUNCTION__, "Copy by const ref");
  SpecializedCopy<const T &>(target, options);
}

template <typename T>
void CopyApi<T>::Copy(T &&target, std::initializer_list<CopyOption> options) {
  env::log_trace(__FUNCTION__, "Copy by move");
  SpecializedCopy<T &&>(std::move(target), options);
}

// template <typename TargetType>
template <typename T>
template <typename TargetType>
void CopyApi<T>::SpecializedCopy(TargetType target,
                                 std::initializer_list<CopyOption> options) {
  T &t = static_cast<T &>(*this);
  t.state_.ExpectsUnlock();
  for (const CopyOption o : options) {
    switch (o) {
    case CopyOption::PreprocessorFlags:
      t.storer_.current_preprocessor_flags =
          std::move(target.storer_.current_preprocessor_flags);
      break;
    case CopyOption::CommonCompileFlags:
      t.storer_.current_common_compile_flags =
          std::move(target.storer_.current_common_compile_flags);
      break;
    case CopyOption::PchCompileFlags:
      t.storer_.current_pch_compile_flags =
          std::move(target.storer_.current_pch_compile_flags);
      break;
    case CopyOption::PchObjectFlags:
      t.storer_.current_pch_object_flags =
          std::move(target.storer_.current_pch_object_flags);
      break;
    case CopyOption::AsmCompileFlags:
      t.storer_.current_asm_compile_flags =
          std::move(target.storer_.current_asm_compile_flags);
      break;
    case CopyOption::CCompileFlags:
      t.storer_.current_c_compile_flags =
          std::move(target.storer_.current_c_compile_flags);
      break;
    case CopyOption::CppCompileFlags:
      t.storer_.current_cpp_compile_flags =
          std::move(target.storer_.current_cpp_compile_flags);
      break;
    case CopyOption::LinkFlags:
      t.storer_.current_link_flags =
          std::move(target.storer_.current_link_flags);
      break;
    case CopyOption::CompileDependencies:
      t.storer_.current_compile_dependencies.user =
          std::move(target.storer_.current_compile_dependencies.user);
      break;
    case CopyOption::LinkDependencies:
      t.storer_.current_link_dependencies.user =
          std::move(target.storer_.current_link_dependencies.user);
      break;
    case CopyOption::SourceFiles:
      t.storer_.current_source_files.user =
          std::move(target.storer_.current_source_files.user);
      break;
    case CopyOption::HeaderFiles:
      t.storer_.current_header_files.user =
          std::move(target.storer_.current_header_files.user);
      break;
    case CopyOption::PchFiles:
      t.storer_.current_pch_files.user =
          std::move(target.storer_.current_pch_files.user);
      break;
    case CopyOption::LibDeps:
      t.storer_.current_lib_deps.user =
          std::move(target.storer_.current_lib_deps.user);
      break;
    case CopyOption::IncludeDirs:
      t.storer_.current_include_dirs =
          std::move(target.storer_.current_include_dirs);
      break;
    case CopyOption::LibDirs:
      t.storer_.current_lib_dirs = std::move(target.storer_.current_lib_dirs);
      break;
    case CopyOption::ExternalLibDeps:
      t.storer_.current_external_lib_deps =
          std::move(target.storer_.current_external_lib_deps);
      break;
    default:
      env::assert_fatal<false>("Invalid Option added");
      break;
    }
  }
}

template class CopyApi<TargetInfo>;

} // namespace buildcc::base
