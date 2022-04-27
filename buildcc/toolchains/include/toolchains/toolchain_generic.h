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

#ifndef TOOLCHAINS_TOOLCHAIN_GENERIC_H_
#define TOOLCHAINS_TOOLCHAIN_GENERIC_H_

#include "toolchain/toolchain.h"

#include "toolchain_gcc.h"
#include "toolchain_mingw.h"
#include "toolchain_msvc.h"

#include "env/storage.h"

namespace buildcc {

class Toolchain_generic {
public:
  // Compile time
  template <ToolchainId id, typename... Params>
  static Toolchain &New(const std::string &identifier, Params &&...params) {
    Toolchain *toolchain{nullptr};

    if constexpr (id == ToolchainId::Gcc) {
      toolchain = AddIf<Toolchain_gcc>(identifier, identifier,
                                       std::forward<Params>(params)...);
    }

    if constexpr (id == ToolchainId::Msvc) {
      toolchain = AddIf<Toolchain_msvc>(identifier, identifier,
                                        std::forward<Params>(params)...);
    }

    if constexpr (id == ToolchainId::MinGW) {
      toolchain = AddIf<Toolchain_mingw>(identifier, identifier,
                                         std::forward<Params>(params)...);
    }

    env::assert_fatal(toolchain, "Toolchain could not be created");
    return *toolchain;
  }

  // Run time
  static Toolchain &New(ToolchainId id, const std::string &identifier,
                        std::optional<ToolchainExecutables> op_executables = {},
                        std::optional<ToolchainConfig> op_config = {}) {
    Toolchain *toolchain{nullptr};
    switch (id) {
    case ToolchainId::Gcc:
      toolchain = AddIf<Toolchain_gcc>(identifier, identifier, op_executables,
                                       op_config);
      break;
    case ToolchainId::Msvc:
      toolchain = AddIf<Toolchain_msvc>(identifier, identifier, op_executables,
                                        op_config);
      break;
    case ToolchainId::MinGW:
      toolchain = AddIf<Toolchain_mingw>(identifier, identifier, op_executables,
                                         op_config);
      break;
    default:
      break;
    }
    env::assert_fatal(toolchain, "Toolchain could not be created");
    return *toolchain;
  }

private:
  template <typename Type, typename... Params>
  static Toolchain *AddIf(const std::string &identifier, Params &&...params) {
    Toolchain *toolchain{nullptr};
    if (!Storage::Contains(identifier)) {
      toolchain =
          &Storage::Add<Type>(identifier, std::forward<Params>(params)...);
    }
    return toolchain;
  }
};

} // namespace buildcc

#endif
