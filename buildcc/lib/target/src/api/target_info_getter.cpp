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

#include "target/api/target_info_getter.h"

#include "target/target_info.h"

namespace buildcc::internal {

// Target Env
template <typename T>
const fs::path &TargetInfoGetter<T>::GetTargetRootDir() const {
  const T &t = static_cast<const T &>(*this);

  return t.env_.GetTargetRootDir();
}

template <typename T>
const fs::path &TargetInfoGetter<T>::GetTargetBuildDir() const {
  const T &t = static_cast<const T &>(*this);

  return t.env_.GetTargetBuildDir();
}

// Target Config
template <typename T>
const TargetConfig &TargetInfoGetter<T>::GetConfig() const {
  const T &t = static_cast<const T &>(*this);

  return t.config_;
}

// Target Storer
template <typename T>
const fs_unordered_set &TargetInfoGetter<T>::GetSourceFiles() const {
  const T &t = static_cast<const T &>(*this);

  return t.user_.sources;
}

template <typename T>
const fs_unordered_set &TargetInfoGetter<T>::GetHeaderFiles() const {
  const T &t = static_cast<const T &>(*this);

  return t.user_.headers;
}

template <typename T>
const fs_unordered_set &TargetInfoGetter<T>::GetPchFiles() const {
  const T &t = static_cast<const T &>(*this);

  return t.user_.pchs;
}

template <typename T>
const std::vector<fs::path> &TargetInfoGetter<T>::GetLibDeps() const {
  const T &t = static_cast<const T &>(*this);

  return t.user_.libs;
}

template <typename T>
const std::vector<std::string> &
TargetInfoGetter<T>::GetExternalLibDeps() const {
  const T &t = static_cast<const T &>(*this);

  return t.user_.external_libs;
}

template <typename T>
const fs_unordered_set &TargetInfoGetter<T>::GetIncludeDirs() const {
  const T &t = static_cast<const T &>(*this);

  return t.user_.include_dirs;
}

template <typename T>
const fs_unordered_set &TargetInfoGetter<T>::GetLibDirs() const {
  const T &t = static_cast<const T &>(*this);

  return t.user_.lib_dirs;
}

template <typename T>
const std::unordered_set<std::string> &
TargetInfoGetter<T>::GetPreprocessorFlags() const {
  const T &t = static_cast<const T &>(*this);

  return t.user_.preprocessor_flags;
}

template <typename T>
const std::unordered_set<std::string> &
TargetInfoGetter<T>::GetCommonCompileFlags() const {
  const T &t = static_cast<const T &>(*this);

  return t.user_.common_compile_flags;
}

template <typename T>
const std::unordered_set<std::string> &
TargetInfoGetter<T>::GetPchCompileFlags() const {
  const T &t = static_cast<const T &>(*this);

  return t.user_.pch_compile_flags;
}

template <typename T>
const std::unordered_set<std::string> &
TargetInfoGetter<T>::GetPchObjectFlags() const {
  const T &t = static_cast<const T &>(*this);

  return t.user_.pch_object_flags;
}

template <typename T>
const std::unordered_set<std::string> &
TargetInfoGetter<T>::GetAsmCompileFlags() const {
  const T &t = static_cast<const T &>(*this);

  return t.user_.asm_compile_flags;
}

template <typename T>
const std::unordered_set<std::string> &
TargetInfoGetter<T>::GetCCompileFlags() const {
  const T &t = static_cast<const T &>(*this);

  return t.user_.c_compile_flags;
}

template <typename T>
const std::unordered_set<std::string> &
TargetInfoGetter<T>::GetCppCompileFlags() const {
  const T &t = static_cast<const T &>(*this);

  return t.user_.cpp_compile_flags;
}

template <typename T>
const std::unordered_set<std::string> &
TargetInfoGetter<T>::GetLinkFlags() const {
  const T &t = static_cast<const T &>(*this);

  return t.user_.link_flags;
}

template <typename T>
const fs_unordered_set &TargetInfoGetter<T>::GetCompileDependencies() const {
  const T &t = static_cast<const T &>(*this);

  return t.user_.compile_dependencies;
}

template <typename T>
const fs_unordered_set &TargetInfoGetter<T>::GetLinkDependencies() const {
  const T &t = static_cast<const T &>(*this);

  return t.user_.link_dependencies;
}

template class TargetInfoGetter<TargetInfo>;

} // namespace buildcc::internal
