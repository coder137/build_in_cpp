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

#include "toolchains/toolchain_infos.h"

#include "toolchains/toolchain_generic.h"

#include "toolchains/toolchain_custom.h"
#include "toolchains/toolchain_gcc.h"
#include "toolchains/toolchain_mingw.h"
#include "toolchains/toolchain_msvc.h"

#include "env/assert_fatal.h"
#include "env/storage.h"

namespace {

template <typename Type, typename... Params>
static buildcc::Toolchain *AddIf(const std::string &identifier,
                                 Params &&...params) {
  buildcc::Toolchain *toolchain{nullptr};
  if (!buildcc::Storage::Contains(identifier)) {
    toolchain = &buildcc::Storage::Add<Type>(identifier,
                                             std::forward<Params>(params)...);
  }
  return toolchain;
}

} // namespace

namespace buildcc {

void Toolchain_custom::Initialize() {
  auto id = GetId();
  RefConfig() = GlobalToolchainMetadata::GetConfig(id);
  SetToolchainInfoCb(GlobalToolchainMetadata::GetInfoCb(id));
}

Toolchain &Toolchain_generic::New(
    ToolchainId id, const std::string &identifier,
    const std::optional<ToolchainExecutables> &op_executables,
    const std::optional<ToolchainConfig> &op_config) {
  Toolchain *toolchain{nullptr};
  switch (id) {
  case ToolchainId::Gcc:
    toolchain =
        AddIf<Toolchain_gcc>(identifier, identifier, op_executables, op_config);
    break;
  case ToolchainId::Msvc:
    toolchain = AddIf<Toolchain_msvc>(identifier, identifier, op_executables,
                                      op_config);
    break;
  case ToolchainId::MinGW:
    toolchain = AddIf<Toolchain_mingw>(identifier, identifier, op_executables,
                                       op_config);
    break;
  case ToolchainId::Clang:
  case ToolchainId::Custom:
    env::assert_fatal(op_executables.has_value(),
                      "ToolchainId::Custom and ToolchainId::Clang require "
                      "executables to be provided");
    toolchain = AddIf<Toolchain_custom>(identifier, id, identifier,
                                        op_executables.value(),
                                        op_config.value_or(ToolchainConfig()));
    break;
  default:
    break;
  }
  env::assert_fatal(toolchain != nullptr, "Toolchain could not be created");
  return *toolchain;
}

} // namespace buildcc
