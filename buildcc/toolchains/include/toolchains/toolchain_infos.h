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

class GlobalToolchainInfo {
public:
  static const ToolchainInfoCb &Get(ToolchainId id);

private:
  static std::unordered_map<ToolchainId, ToolchainInfoCb>
      global_toolchain_info_func_;
};

} // namespace buildcc

#endif
