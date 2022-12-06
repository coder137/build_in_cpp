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

#ifndef TOOLCHAIN_API_FLAG_API_H_
#define TOOLCHAIN_API_FLAG_API_H_

#include <string>
#include <vector>

namespace buildcc::internal {

// Requires
// TargetSchema
template <typename T> class FlagApi {
public:
  void AddPreprocessorFlag(const std::string &flag) {
    auto &t = static_cast<T &>(*this);
    t.user_.preprocessor_flags.push_back(flag);
  }

  void AddCommonCompileFlag(const std::string &flag) {
    auto &t = static_cast<T &>(*this);
    t.user_.common_compile_flags.push_back(flag);
  }

  void AddPchCompileFlag(const std::string &flag) {
    auto &t = static_cast<T &>(*this);
    t.user_.pch_compile_flags.push_back(flag);
  }

  void AddPchObjectFlag(const std::string &flag) {
    auto &t = static_cast<T &>(*this);
    t.user_.pch_object_flags.push_back(flag);
  }

  void AddAsmCompileFlag(const std::string &flag) {
    auto &t = static_cast<T &>(*this);
    t.user_.asm_compile_flags.push_back(flag);
  }

  void AddCCompileFlag(const std::string &flag) {
    auto &t = static_cast<T &>(*this);
    t.user_.c_compile_flags.push_back(flag);
  }

  void AddCppCompileFlag(const std::string &flag) {
    auto &t = static_cast<T &>(*this);
    t.user_.cpp_compile_flags.push_back(flag);
  }

  void AddLinkFlag(const std::string &flag) {
    auto &t = static_cast<T &>(*this);
    t.user_.link_flags.push_back(flag);
  }

  // Getters
  const std::vector<std::string> &GetPreprocessorFlags() const {
    const auto &t = static_cast<const T &>(*this);
    return t.user_.preprocessor_flags;
  }

  const std::vector<std::string> &GetCommonCompileFlags() const {
    const auto &t = static_cast<const T &>(*this);
    return t.user_.common_compile_flags;
  }

  const std::vector<std::string> &GetPchCompileFlags() const {
    const auto &t = static_cast<const T &>(*this);
    return t.user_.pch_compile_flags;
  }

  const std::vector<std::string> &GetPchObjectFlags() const {
    const auto &t = static_cast<const T &>(*this);
    return t.user_.pch_object_flags;
  }

  const std::vector<std::string> &GetAsmCompileFlags() const {
    const auto &t = static_cast<const T &>(*this);
    return t.user_.asm_compile_flags;
  }

  const std::vector<std::string> &GetCCompileFlags() const {
    const auto &t = static_cast<const T &>(*this);
    return t.user_.c_compile_flags;
  }

  const std::vector<std::string> &GetCppCompileFlags() const {
    const auto &t = static_cast<const T &>(*this);
    return t.user_.cpp_compile_flags;
  }

  const std::vector<std::string> &GetLinkFlags() const {
    const auto &t = static_cast<const T &>(*this);
    return t.user_.link_flags;
  }
};

} // namespace buildcc::internal

#endif
