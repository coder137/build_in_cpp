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

#ifndef TOOLCHAINS_TOOLCHAIN_INFOS_H_
#define TOOLCHAINS_TOOLCHAIN_INFOS_H_

#include <unordered_map>

#include "toolchain/toolchain.h"

namespace buildcc {

class GlobalToolchainMetadata {
public:
  static const ToolchainConfig &GetConfig(ToolchainId id);
  static const ToolchainInfoCb &GetInfoCb(ToolchainId id);

private:
  struct ToolchainMetadata {
    ToolchainMetadata(const ToolchainConfig &config, const ToolchainInfoCb &cb)
        : config_(config), cb_(cb) {}

    ToolchainConfig config_;
    ToolchainInfoCb cb_;
  };

private:
  static void Expect(ToolchainId id);
  static const ToolchainMetadata &Get(ToolchainId id);

private:
  static std::unordered_map<ToolchainId, ToolchainMetadata>
      global_toolchain_metadata_;
};

} // namespace buildcc

#endif
