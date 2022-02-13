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

#ifndef TARGET_API_TARGET_INFO_GETTER_H_
#define TARGET_API_TARGET_INFO_GETTER_H_

#include "schema/path.h"

#include "target/common/target_config.h"
#include "target/common/target_state.h"

namespace buildcc::internal {

// Requires
// - TargetStorer
// - TargetState
// - TargetEnv
// - TargetConfig
template <typename T> class TargetInfoGetter {
public:
  // Target State
  const TargetState &GetState() const;
  bool IsBuilt() const;
  bool IsLocked() const;

  // Target Env
  const fs::path &GetTargetRootDir() const;
  const fs::path &GetTargetBuildDir() const;

  // Target Config
  const TargetConfig &GetConfig() const;

  // Target Storer
  const fs_unordered_set &GetSourceFiles() const;
  const fs_unordered_set &GetHeaderFiles() const;
  const fs_unordered_set &GetPchFiles() const;
  const std::vector<fs::path> &GetLibDeps() const;
  const std::vector<std::string> &GetExternalLibDeps() const;
  const fs_unordered_set &GetIncludeDirs() const;
  const fs_unordered_set &GetLibDirs() const;
  const std::unordered_set<std::string> &GetPreprocessorFlags() const;
  const std::unordered_set<std::string> &GetCommonCompileFlags() const;
  const std::unordered_set<std::string> &GetPchCompileFlags() const;
  const std::unordered_set<std::string> &GetPchObjectFlags() const;
  const std::unordered_set<std::string> &GetAsmCompileFlags() const;
  const std::unordered_set<std::string> &GetCCompileFlags() const;
  const std::unordered_set<std::string> &GetCppCompileFlags() const;
  const std::unordered_set<std::string> &GetLinkFlags() const;
  const fs_unordered_set &GetCompileDependencies() const;
  const fs_unordered_set &GetLinkDependencies() const;
};

} // namespace buildcc::internal

#endif
