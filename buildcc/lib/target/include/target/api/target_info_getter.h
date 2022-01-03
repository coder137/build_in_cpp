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

#ifndef TARGET_API_TARGET_INFO_GETTER_H_
#define TARGET_API_TARGET_INFO_GETTER_H_

#include "target/common/path.h"

#include "target/common/target_config.h"
#include "target/common/target_state.h"

namespace buildcc::base {

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
  const fs_unordered_set &GetCurrentHeaderFiles() const;
  const fs_unordered_set &GetCurrentPchFiles() const;
  const std::vector<fs::path> &GetTargetLibDeps() const;
  const std::vector<std::string> &GetCurrentExternalLibDeps() const;
  const fs_unordered_set &GetCurrentIncludeDirs() const;
  const fs_unordered_set &GetCurrentLibDirs() const;
  const std::unordered_set<std::string> &GetCurrentPreprocessorFlags() const;
  const std::unordered_set<std::string> &GetCurrentCommonCompileFlags() const;
  const std::unordered_set<std::string> &GetCurrentPchCompileFlags() const;
  const std::unordered_set<std::string> &GetCurrentPchObjectFlags() const;
  const std::unordered_set<std::string> &GetCurrentAsmCompileFlags() const;
  const std::unordered_set<std::string> &GetCurrentCCompileFlags() const;
  const std::unordered_set<std::string> &GetCurrentCppCompileFlags() const;
  const std::unordered_set<std::string> &GetCurrentLinkFlags() const;
  const fs_unordered_set &GetCurrentCompileDependencies() const;
  const fs_unordered_set &GetCurrentLinkDependencies() const;
};

} // namespace buildcc::base

#endif
