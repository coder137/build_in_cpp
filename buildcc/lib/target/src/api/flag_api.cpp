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

#include "target/api/flag_api.h"

#include "target/target_info.h"

namespace buildcc::internal {

template <typename T>
void FlagApi<T>::AddPreprocessorFlag(const std::string &flag) {
  T &t = static_cast<T &>(*this);

  t.lock_.ExpectsUnlock(__FUNCTION__);
  t.user_.preprocessor_flags.insert(flag);
}
template <typename T>
void FlagApi<T>::AddCommonCompileFlag(const std::string &flag) {
  T &t = static_cast<T &>(*this);

  t.lock_.ExpectsUnlock(__FUNCTION__);
  t.user_.common_compile_flags.insert(flag);
}
template <typename T>
void FlagApi<T>::AddPchCompileFlag(const std::string &flag) {
  T &t = static_cast<T &>(*this);

  t.lock_.ExpectsUnlock(__FUNCTION__);
  t.user_.pch_compile_flags.insert(flag);
}
template <typename T>
void FlagApi<T>::AddPchObjectFlag(const std::string &flag) {
  T &t = static_cast<T &>(*this);

  t.lock_.ExpectsUnlock(__FUNCTION__);
  t.user_.pch_object_flags.insert(flag);
}
template <typename T>
void FlagApi<T>::AddAsmCompileFlag(const std::string &flag) {
  T &t = static_cast<T &>(*this);

  t.lock_.ExpectsUnlock(__FUNCTION__);
  t.user_.asm_compile_flags.insert(flag);
}
template <typename T>
void FlagApi<T>::AddCCompileFlag(const std::string &flag) {
  T &t = static_cast<T &>(*this);

  t.lock_.ExpectsUnlock(__FUNCTION__);
  t.user_.c_compile_flags.insert(flag);
}
template <typename T>
void FlagApi<T>::AddCppCompileFlag(const std::string &flag) {
  T &t = static_cast<T &>(*this);

  t.lock_.ExpectsUnlock(__FUNCTION__);
  t.user_.cpp_compile_flags.insert(flag);
}
template <typename T> void FlagApi<T>::AddLinkFlag(const std::string &flag) {
  T &t = static_cast<T &>(*this);

  t.lock_.ExpectsUnlock(__FUNCTION__);
  t.user_.link_flags.insert(flag);
}

template class FlagApi<TargetInfo>;

} // namespace buildcc::internal
