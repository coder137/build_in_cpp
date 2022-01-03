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

#include "target/api/target_info_getter.h"

#include "target/target_info.h"

namespace buildcc::base {

// Target State
template <typename T> const TargetState &TargetInfoGetter<T>::GetState() const {
  const T &t = static_cast<const T &>(*this);

  return t.state_;
}

template <typename T> bool TargetInfoGetter<T>::IsBuilt() const {
  const T &t = static_cast<const T &>(*this);

  return t.state_.build;
}

template <typename T> bool TargetInfoGetter<T>::IsLocked() const {
  const T &t = static_cast<const T &>(*this);

  return t.state_.lock;
}

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

  return t.storer_.current_source_files.user;
}

template <typename T>
const fs_unordered_set &TargetInfoGetter<T>::GetCurrentHeaderFiles() const {
  const T &t = static_cast<const T &>(*this);

  return t.storer_.current_header_files.user;
}

template <typename T>
const fs_unordered_set &TargetInfoGetter<T>::GetCurrentPchFiles() const {
  const T &t = static_cast<const T &>(*this);

  return t.storer_.current_pch_files.user;
}

template <typename T>
const std::vector<fs::path> &TargetInfoGetter<T>::GetTargetLibDeps() const {
  const T &t = static_cast<const T &>(*this);

  return t.storer_.current_user_lib_deps;
}

template <typename T>
const std::vector<std::string> &
TargetInfoGetter<T>::GetCurrentExternalLibDeps() const {
  const T &t = static_cast<const T &>(*this);

  return t.storer_.current_user_external_lib_deps;
}

template <typename T>
const fs_unordered_set &TargetInfoGetter<T>::GetCurrentIncludeDirs() const {
  const T &t = static_cast<const T &>(*this);

  return t.storer_.current_include_dirs;
}

template <typename T>
const fs_unordered_set &TargetInfoGetter<T>::GetCurrentLibDirs() const {
  const T &t = static_cast<const T &>(*this);

  return t.storer_.current_lib_dirs;
}

template <typename T>
const std::unordered_set<std::string> &
TargetInfoGetter<T>::GetCurrentPreprocessorFlags() const {
  const T &t = static_cast<const T &>(*this);

  return t.storer_.current_preprocessor_flags;
}

template <typename T>
const std::unordered_set<std::string> &
TargetInfoGetter<T>::GetCurrentCommonCompileFlags() const {
  const T &t = static_cast<const T &>(*this);

  return t.storer_.current_common_compile_flags;
}

template <typename T>
const std::unordered_set<std::string> &
TargetInfoGetter<T>::GetCurrentPchCompileFlags() const {
  const T &t = static_cast<const T &>(*this);

  return t.storer_.current_pch_compile_flags;
}

template <typename T>
const std::unordered_set<std::string> &
TargetInfoGetter<T>::GetCurrentPchObjectFlags() const {
  const T &t = static_cast<const T &>(*this);

  return t.storer_.current_pch_object_flags;
}

template <typename T>
const std::unordered_set<std::string> &
TargetInfoGetter<T>::GetCurrentAsmCompileFlags() const {
  const T &t = static_cast<const T &>(*this);

  return t.storer_.current_asm_compile_flags;
}

template <typename T>
const std::unordered_set<std::string> &
TargetInfoGetter<T>::GetCurrentCCompileFlags() const {
  const T &t = static_cast<const T &>(*this);

  return t.storer_.current_c_compile_flags;
}

template <typename T>
const std::unordered_set<std::string> &
TargetInfoGetter<T>::GetCurrentCppCompileFlags() const {
  const T &t = static_cast<const T &>(*this);

  return t.storer_.current_cpp_compile_flags;
}

template <typename T>
const std::unordered_set<std::string> &
TargetInfoGetter<T>::GetCurrentLinkFlags() const {
  const T &t = static_cast<const T &>(*this);

  return t.storer_.current_link_flags;
}

template <typename T>
const fs_unordered_set &
TargetInfoGetter<T>::GetCurrentCompileDependencies() const {
  const T &t = static_cast<const T &>(*this);

  return t.storer_.current_compile_dependencies.user;
}

template <typename T>
const fs_unordered_set &
TargetInfoGetter<T>::GetCurrentLinkDependencies() const {
  const T &t = static_cast<const T &>(*this);

  return t.storer_.current_link_dependencies.user;
}

template class TargetInfoGetter<TargetInfo>;

} // namespace buildcc::base
