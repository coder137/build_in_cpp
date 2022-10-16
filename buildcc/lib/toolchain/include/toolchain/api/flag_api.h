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
#include <unordered_set>

namespace buildcc::internal {

// Requires
// TargetSchema
template <typename T> class FlagApi {
public:
  void AddPreprocessorFlag(const std::string &flag);
  void AddCommonCompileFlag(const std::string &flag);
  void AddPchCompileFlag(const std::string &flag);
  void AddPchObjectFlag(const std::string &flag);
  void AddAsmCompileFlag(const std::string &flag);
  void AddCCompileFlag(const std::string &flag);
  void AddCppCompileFlag(const std::string &flag);
  void AddLinkFlag(const std::string &flag);

  // Getters
  const std::unordered_set<std::string> &GetPreprocessorFlags() const;
  const std::unordered_set<std::string> &GetCommonCompileFlags() const;
  const std::unordered_set<std::string> &GetPchCompileFlags() const;
  const std::unordered_set<std::string> &GetPchObjectFlags() const;
  const std::unordered_set<std::string> &GetAsmCompileFlags() const;
  const std::unordered_set<std::string> &GetCCompileFlags() const;
  const std::unordered_set<std::string> &GetCppCompileFlags() const;
  const std::unordered_set<std::string> &GetLinkFlags() const;
};

} // namespace buildcc::internal

#endif
