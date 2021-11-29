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

#include "env/assert_fatal.h"

namespace buildcc::base {

template <typename T>
void SyncApi<T>::Copy(const T &target,
                      std::initializer_list<SyncOption> options) {
  env::log_trace(__FUNCTION__, "Copy/Replace by const ref");
  SpecializedCopy<const T &>(target, options);
}

template <typename T>
void SyncApi<T>::Copy(T &&target, std::initializer_list<SyncOption> options) {
  env::log_trace(__FUNCTION__, "Copy/Replace by move");
  SpecializedCopy<T &&>(std::move(target), options);
}

// template <typename TargetType>
template <typename T>
template <typename TargetType>
void SyncApi<T>::SpecializedCopy(TargetType target,
                                 std::initializer_list<SyncOption> options) {
  T &t = static_cast<T &>(*this);
  t.state_.ExpectsUnlock();
  for (const SyncOption o : options) {
    switch (o) {
    case SyncOption::PreprocessorFlags:
      t.storer_.current_preprocessor_flags =
          std::move(target.storer_.current_preprocessor_flags);
      break;
    case SyncOption::CommonCompileFlags:
      t.storer_.current_common_compile_flags =
          std::move(target.storer_.current_common_compile_flags);
      break;
    case SyncOption::PchCompileFlags:
      t.storer_.current_pch_compile_flags =
          std::move(target.storer_.current_pch_compile_flags);
      break;
    case SyncOption::PchObjectFlags:
      t.storer_.current_pch_object_flags =
          std::move(target.storer_.current_pch_object_flags);
      break;
    case SyncOption::AsmCompileFlags:
      t.storer_.current_asm_compile_flags =
          std::move(target.storer_.current_asm_compile_flags);
      break;
    case SyncOption::CCompileFlags:
      t.storer_.current_c_compile_flags =
          std::move(target.storer_.current_c_compile_flags);
      break;
    case SyncOption::CppCompileFlags:
      t.storer_.current_cpp_compile_flags =
          std::move(target.storer_.current_cpp_compile_flags);
      break;
    case SyncOption::LinkFlags:
      t.storer_.current_link_flags =
          std::move(target.storer_.current_link_flags);
      break;
    case SyncOption::CompileDependencies:
      t.storer_.current_compile_dependencies.user =
          std::move(target.storer_.current_compile_dependencies.user);
      break;
    case SyncOption::LinkDependencies:
      t.storer_.current_link_dependencies.user =
          std::move(target.storer_.current_link_dependencies.user);
      break;
    case SyncOption::SourceFiles:
      t.storer_.current_source_files.user =
          std::move(target.storer_.current_source_files.user);
      break;
    case SyncOption::HeaderFiles:
      t.storer_.current_header_files.user =
          std::move(target.storer_.current_header_files.user);
      break;
    case SyncOption::PchFiles:
      t.storer_.current_pch_files.user =
          std::move(target.storer_.current_pch_files.user);
      break;
    case SyncOption::LibDeps:
      t.storer_.current_lib_deps.user =
          std::move(target.storer_.current_lib_deps.user);
      break;
    case SyncOption::IncludeDirs:
      t.storer_.current_include_dirs =
          std::move(target.storer_.current_include_dirs);
      break;
    case SyncOption::LibDirs:
      t.storer_.current_lib_dirs = std::move(target.storer_.current_lib_dirs);
      break;
    case SyncOption::ExternalLibDeps:
      t.storer_.current_external_lib_deps =
          std::move(target.storer_.current_external_lib_deps);
      break;
    default:
      env::assert_fatal<false>("Invalid Option added");
      break;
    }
  }
}

template <typename T>
void SyncApi<T>::Insert(const T &target,
                        std::initializer_list<SyncOption> options) {
  env::log_trace(__FUNCTION__, "Insert by const ref");
  SpecializedInsert<const T &>(target, options);
}

template <typename T>
void SyncApi<T>::Insert(T &&target, std::initializer_list<SyncOption> options) {
  env::log_trace(__FUNCTION__, "Insert by move");
  SpecializedInsert<T &&>(std::move(target), options);
}

template <typename T>
template <typename TargetType>
void SyncApi<T>::SpecializedInsert(TargetType target,
                                   std::initializer_list<SyncOption> options) {
  T &t = static_cast<T &>(*this);
  t.state_.ExpectsUnlock();
  for (const SyncOption o : options) {
    switch (o) {
    case SyncOption::PreprocessorFlags:
      t.storer_.current_preprocessor_flags.insert(
          std::make_move_iterator(
              target.storer_.current_preprocessor_flags.begin()),
          std::make_move_iterator(
              target.storer_.current_preprocessor_flags.end()));
      break;
    case SyncOption::CommonCompileFlags:
      t.storer_.current_common_compile_flags.insert(
          std::make_move_iterator(
              target.storer_.current_common_compile_flags.begin()),
          std::make_move_iterator(
              target.storer_.current_common_compile_flags.end()));
      break;
    case SyncOption::PchCompileFlags:
      t.storer_.current_pch_compile_flags.insert(
          std::make_move_iterator(
              target.storer_.current_pch_compile_flags.begin()),
          std::make_move_iterator(
              target.storer_.current_pch_compile_flags.end()));
      break;
    case SyncOption::PchObjectFlags:
      t.storer_.current_pch_object_flags.insert(
          std::make_move_iterator(
              target.storer_.current_pch_object_flags.begin()),
          std::make_move_iterator(
              target.storer_.current_pch_object_flags.end()));
      break;
    case SyncOption::AsmCompileFlags:
      t.storer_.current_asm_compile_flags.insert(
          std::make_move_iterator(
              target.storer_.current_asm_compile_flags.begin()),
          std::make_move_iterator(
              target.storer_.current_asm_compile_flags.end()));
      break;
    case SyncOption::CCompileFlags:
      t.storer_.current_c_compile_flags.insert(
          std::make_move_iterator(
              target.storer_.current_c_compile_flags.begin()),
          std::make_move_iterator(
              target.storer_.current_c_compile_flags.end()));
      break;
    case SyncOption::CppCompileFlags:
      t.storer_.current_cpp_compile_flags.insert(
          std::make_move_iterator(
              target.storer_.current_cpp_compile_flags.begin()),
          std::make_move_iterator(
              target.storer_.current_cpp_compile_flags.end()));
      break;
    case SyncOption::LinkFlags:
      t.storer_.current_link_flags.insert(
          std::make_move_iterator(target.storer_.current_link_flags.begin()),
          std::make_move_iterator(target.storer_.current_link_flags.end()));
      break;
    case SyncOption::CompileDependencies:
      t.storer_.current_compile_dependencies.user.insert(
          std::make_move_iterator(
              target.storer_.current_compile_dependencies.user.begin()),
          std::make_move_iterator(
              target.storer_.current_compile_dependencies.user.end()));
      break;
    case SyncOption::LinkDependencies:
      t.storer_.current_link_dependencies.user.insert(
          std::make_move_iterator(
              target.storer_.current_link_dependencies.user.begin()),
          std::make_move_iterator(
              target.storer_.current_link_dependencies.user.end()));
      break;
    case SyncOption::SourceFiles:
      t.storer_.current_source_files.user.insert(
          std::make_move_iterator(
              target.storer_.current_source_files.user.begin()),
          std::make_move_iterator(
              target.storer_.current_source_files.user.end()));
      break;
    case SyncOption::HeaderFiles:
      t.storer_.current_header_files.user.insert(
          std::make_move_iterator(
              target.storer_.current_header_files.user.begin()),
          std::make_move_iterator(
              target.storer_.current_header_files.user.end()));
      break;
    case SyncOption::PchFiles:
      t.storer_.current_pch_files.user.insert(
          std::make_move_iterator(
              target.storer_.current_pch_files.user.begin()),
          std::make_move_iterator(target.storer_.current_pch_files.user.end()));
      break;
    case SyncOption::LibDeps:
      t.storer_.current_lib_deps.user.insert(
          std::make_move_iterator(target.storer_.current_lib_deps.user.begin()),
          std::make_move_iterator(target.storer_.current_lib_deps.user.end()));
      break;
    case SyncOption::IncludeDirs:
      t.storer_.current_include_dirs.insert(
          std::make_move_iterator(target.storer_.current_include_dirs.begin()),
          std::make_move_iterator(target.storer_.current_include_dirs.end()));
      break;
    case SyncOption::LibDirs:
      t.storer_.current_lib_dirs.insert(
          std::make_move_iterator(target.storer_.current_lib_dirs.begin()),
          std::make_move_iterator(target.storer_.current_lib_dirs.end()));
      break;
    case SyncOption::ExternalLibDeps:
      t.storer_.current_external_lib_deps.insert(
          std::make_move_iterator(
              target.storer_.current_external_lib_deps.begin()),
          std::make_move_iterator(
              target.storer_.current_external_lib_deps.end()));
      break;
    default:
      env::assert_fatal<false>("Invalid Option added");
      break;
    }
  }
}

template class SyncApi<TargetInfo>;

} // namespace buildcc::base
