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

#include "target/api/sync_api.h"

#include "target/target_info.h"

#include "env/assert_fatal.h"

namespace buildcc::internal {

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
  auto &t = static_cast<T &>(*this);
  for (const SyncOption o : options) {
    switch (o) {
    case SyncOption::PreprocessorFlags:
      t.user_.preprocessor_flags = std::move(target.user_.preprocessor_flags);
      break;
    case SyncOption::CommonCompileFlags:
      t.user_.common_compile_flags =
          std::move(target.user_.common_compile_flags);
      break;
    case SyncOption::PchCompileFlags:
      t.user_.pch_compile_flags = std::move(target.user_.pch_compile_flags);
      break;
    case SyncOption::PchObjectFlags:
      t.user_.pch_object_flags = std::move(target.user_.pch_object_flags);
      break;
    case SyncOption::AsmCompileFlags:
      t.user_.asm_compile_flags = std::move(target.user_.asm_compile_flags);
      break;
    case SyncOption::CCompileFlags:
      t.user_.c_compile_flags = std::move(target.user_.c_compile_flags);
      break;
    case SyncOption::CppCompileFlags:
      t.user_.cpp_compile_flags = std::move(target.user_.cpp_compile_flags);
      break;
    case SyncOption::LinkFlags:
      t.user_.link_flags = std::move(target.user_.link_flags);
      break;
    case SyncOption::CompileDependencies:
      t.user_.compile_dependencies =
          std::move(target.user_.compile_dependencies);
      break;
    case SyncOption::LinkDependencies:
      t.user_.link_dependencies = std::move(target.user_.link_dependencies);
      break;
    case SyncOption::SourceFiles:
      t.user_.sources = std::move(target.user_.sources);
      break;
    case SyncOption::HeaderFiles:
      t.user_.headers = std::move(target.user_.headers);
      break;
    case SyncOption::PchFiles:
      t.user_.pchs = std::move(target.user_.pchs);
      break;
    case SyncOption::LibDeps:
      t.user_.libs = std::move(target.user_.libs);
      break;
    case SyncOption::IncludeDirs:
      t.user_.include_dirs = std::move(target.user_.include_dirs);
      break;
    case SyncOption::LibDirs:
      t.user_.lib_dirs = std::move(target.user_.lib_dirs);
      break;
    case SyncOption::ExternalLibDeps:
      t.user_.external_libs = std::move(target.user_.external_libs);
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
  auto &t = static_cast<T &>(*this);
  for (const SyncOption o : options) {
    switch (o) {
    case SyncOption::PreprocessorFlags:
      t.user_.preprocessor_flags.insert(
          t.user_.preprocessor_flags.end(),
          std::make_move_iterator(target.user_.preprocessor_flags.begin()),
          std::make_move_iterator(target.user_.preprocessor_flags.end()));
      break;
    case SyncOption::CommonCompileFlags:
      t.user_.common_compile_flags.insert(
          t.user_.common_compile_flags.end(),
          std::make_move_iterator(target.user_.common_compile_flags.begin()),
          std::make_move_iterator(target.user_.common_compile_flags.end()));
      break;
    case SyncOption::PchCompileFlags:
      t.user_.pch_compile_flags.insert(
          t.user_.pch_compile_flags.end(),
          std::make_move_iterator(target.user_.pch_compile_flags.begin()),
          std::make_move_iterator(target.user_.pch_compile_flags.end()));
      break;
    case SyncOption::PchObjectFlags:
      t.user_.pch_object_flags.insert(
          t.user_.pch_object_flags.end(),
          std::make_move_iterator(target.user_.pch_object_flags.begin()),
          std::make_move_iterator(target.user_.pch_object_flags.end()));
      break;
    case SyncOption::AsmCompileFlags:
      t.user_.asm_compile_flags.insert(
          t.user_.asm_compile_flags.end(),
          std::make_move_iterator(target.user_.asm_compile_flags.begin()),
          std::make_move_iterator(target.user_.asm_compile_flags.end()));
      break;
    case SyncOption::CCompileFlags:
      t.user_.c_compile_flags.insert(
          t.user_.c_compile_flags.end(),
          std::make_move_iterator(target.user_.c_compile_flags.begin()),
          std::make_move_iterator(target.user_.c_compile_flags.end()));
      break;
    case SyncOption::CppCompileFlags:
      t.user_.cpp_compile_flags.insert(
          t.user_.cpp_compile_flags.end(),
          std::make_move_iterator(target.user_.cpp_compile_flags.begin()),
          std::make_move_iterator(target.user_.cpp_compile_flags.end()));
      break;
    case SyncOption::LinkFlags:
      t.user_.link_flags.insert(
          t.user_.link_flags.end(),
          std::make_move_iterator(target.user_.link_flags.begin()),
          std::make_move_iterator(target.user_.link_flags.end()));
      break;
    case SyncOption::CompileDependencies:
      t.user_.compile_dependencies.Insert(
          std::move(target.user_.compile_dependencies));
      break;
    case SyncOption::LinkDependencies:
      t.user_.link_dependencies.Insert(
          std::move(target.user_.link_dependencies));
      break;
    case SyncOption::SourceFiles:
      t.user_.sources.insert(
          std::make_move_iterator(target.user_.sources.begin()),
          std::make_move_iterator(target.user_.sources.end()));
      break;
    case SyncOption::HeaderFiles:
      t.user_.headers.Insert(std::move(target.user_.headers));
      break;
    case SyncOption::PchFiles:
      t.user_.pchs.insert(std::make_move_iterator(target.user_.pchs.begin()),
                          std::make_move_iterator(target.user_.pchs.end()));
      break;
    case SyncOption::LibDeps:
      t.user_.libs.Insert(std::move(target.user_.libs));
      break;
    case SyncOption::IncludeDirs:
      t.user_.include_dirs.Insert(std::move(target.user_.include_dirs));
      break;
    case SyncOption::LibDirs:
      t.user_.lib_dirs.Insert(std::move(target.user_.lib_dirs));
      break;
    case SyncOption::ExternalLibDeps:
      t.user_.external_libs.insert(
          t.user_.external_libs.end(),
          std::make_move_iterator(target.user_.external_libs.begin()),
          std::make_move_iterator(target.user_.external_libs.end()));
      break;
    default:
      env::assert_fatal<false>("Invalid Option added");
      break;
    }
  }
}

template class SyncApi<TargetInfo>;

} // namespace buildcc::internal
