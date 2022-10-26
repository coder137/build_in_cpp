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

namespace buildcc {

class Toolchain_custom : public Toolchain {
public:
  // Run time basic constructor
  Toolchain_custom(ToolchainId id, const std::string &name,
                   const ToolchainExecutables &executables,
                   const env::optional<ToolchainConfig> &op_config = {})
      : Toolchain(id, name, executables,
                  op_config.value_or(ToolchainConfig())) {
    Initialize();
  }

  virtual ~Toolchain_custom() = default;
  Toolchain_custom(const Toolchain_custom &) = delete;

private:
  void Initialize();
};

} // namespace buildcc

#endif
