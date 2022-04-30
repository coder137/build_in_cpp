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

#include "toolchains/toolchain_gcc.h"
#include "toolchains/toolchain_mingw.h"

#include "toolchains/toolchain_infos.h"

#include "env/command.h"

namespace buildcc {

void Toolchain_gcc::Initialize() {
  RefConfig() = GlobalToolchainMetadata::GetConfig(ToolchainId::Gcc);
}

std::optional<ToolchainCompilerInfo>
Toolchain_gcc::GetToolchainInfo(const ToolchainExecutables &executables) const {
  return GlobalToolchainMetadata::GetInfoCb(ToolchainId::Gcc)(executables);
}

void Toolchain_mingw::Initialize() {
  RefConfig() = GlobalToolchainMetadata::GetConfig(ToolchainId::Gcc);
}

std::optional<ToolchainCompilerInfo> Toolchain_mingw::GetToolchainInfo(
    const ToolchainExecutables &executables) const {
  return GlobalToolchainMetadata::GetInfoCb(ToolchainId::MinGW)(executables);
}

} // namespace buildcc
