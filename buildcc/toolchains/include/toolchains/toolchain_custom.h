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

#ifndef TOOLCHAINS_TOOLCHAIN_CUSTOM_H_
#define TOOLCHAINS_TOOLCHAIN_CUSTOM_H_

#include "toolchain/toolchain.h"

#include <functional>

namespace buildcc {

class Toolchain_custom : public Toolchain {
public:
  Toolchain_custom(const std::string &name,
                   const ToolchainExecutables &executables,
                   const ToolchainConfig &config = ToolchainConfig())
      : Toolchain(ToolchainId::Custom, name, executables, config) {}

  void SetToolchainInfoFunc(const ToolchainInfoFunc &cb) { info_func_ = cb; }

private:
  std::optional<ToolchainCompilerInfo>
  GetToolchainInfo(const ToolchainExecutables &executables) const override {
    if (info_func_) {
      return info_func_(executables);
    }
    return {};
  }

private:
  ToolchainInfoFunc info_func_;
};

} // namespace buildcc

#endif
