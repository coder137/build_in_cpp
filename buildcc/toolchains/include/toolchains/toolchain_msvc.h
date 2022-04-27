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

#ifndef TOOLCHAINS_TOOLCHAIN_MSVC_H_
#define TOOLCHAINS_TOOLCHAIN_MSVC_H_

#include "toolchain/toolchain.h"

namespace buildcc {

/**
 * @brief Generic GCC Toolchain <br>
 * id = ToolchainId::Msvc <br>
 * name = "msvc" <br>
 * asm_compiler = "cl" <br>
 * c_compiler = "cl" <br>
 * cpp_compiler = "cl" <br>
 * archiver = "lib" <br>
 * linker = "link" <br>
 */
class Toolchain_msvc : public Toolchain {
public:
  // Run time basic constructor
  Toolchain_msvc(const std::string &name = "msvc",
                 std::optional<ToolchainExecutables> op_executables = {},
                 std::optional<ToolchainConfig> op_config = {})
      : Toolchain(ToolchainId::Gcc, name,
                  op_executables.value_or(
                      ToolchainExecutables("cl", "cl", "cl", "lib", "link")),
                  op_config.value_or(ToolchainConfig())) {}

  virtual ~Toolchain_msvc() = default;
  Toolchain_msvc(const Toolchain_msvc &gcc) = delete;

private:
  void UpdateConfig(ToolchainConfig &config) override;
  std::optional<ToolchainCompilerInfo>
  GetToolchainInfo(const ToolchainExecutables &executables) const override;
};

} // namespace buildcc

#endif
