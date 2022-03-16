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

#include "target/target_info.h"

namespace buildcc {

// PRIVATE

void TargetInfo::Initialize() {
  toolchain_.GetLockInfo().ExpectsLock(__FUNCTION__);

  std::for_each(toolchain_.GetPreprocessorFlags().begin(),
                toolchain_.GetPreprocessorFlags().end(),
                [&](const std::string &flag) { AddPreprocessorFlag(flag); });
  std::for_each(toolchain_.GetCommonCompileFlags().begin(),
                toolchain_.GetCommonCompileFlags().end(),
                [&](const std::string &flag) { AddCommonCompileFlag(flag); });
  std::for_each(toolchain_.GetPchCompileFlags().begin(),
                toolchain_.GetPchCompileFlags().end(),
                [&](const std::string &flag) { AddPchCompileFlag(flag); });
  std::for_each(toolchain_.GetPchObjectFlags().begin(),
                toolchain_.GetPchObjectFlags().end(),
                [&](const std::string &flag) { AddPchObjectFlag(flag); });
  std::for_each(toolchain_.GetAsmCompileFlags().begin(),
                toolchain_.GetAsmCompileFlags().end(),
                [&](const std::string &flag) { AddAsmCompileFlag(flag); });
  std::for_each(toolchain_.GetCCompileFlags().begin(),
                toolchain_.GetCCompileFlags().end(),
                [&](const std::string &flag) { AddCCompileFlag(flag); });
  std::for_each(toolchain_.GetCppCompileFlags().begin(),
                toolchain_.GetCppCompileFlags().end(),
                [&](const std::string &flag) { AddCppCompileFlag(flag); });
  std::for_each(toolchain_.GetLinkFlags().begin(),
                toolchain_.GetLinkFlags().end(),
                [&](const std::string &flag) { AddLinkFlag(flag); });
}

} // namespace buildcc
