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

#ifndef TOOLCHAINS_TOOLCHAIN_MINGW_H_
#define TOOLCHAINS_TOOLCHAIN_MINGW_H_

#include "toolchain/toolchain.h"

#include "toolchain_gcc.h"

namespace buildcc {

/**
 * @brief Generic MinGW Toolchain <br>
 * id = ToolchainId::MinGW <br>
 * name = "gcc" <br>
 * asm_compiler = "as" <br>
 * c_compiler = "gcc" <br>
 * cpp_compiler = "g++" <br>
 * archiver = "ar" <br>
 * linker = "ld" <br>
 */
class Toolchain_mingw : public Toolchain {
public:
  // Run time basic constructor
  Toolchain_mingw(const std::string &name = "gcc",
                  std::optional<ToolchainExecutables> op_executables = {},
                  std::optional<ToolchainConfig> op_config = {})
      : Toolchain(ToolchainId::MinGW, name,
                  op_executables.value_or(
                      ToolchainExecutables("as", "gcc", "g++", "ar", "ld")),
                  op_config.value_or(ToolchainConfig())) {
    Initialize();
  }

  virtual ~Toolchain_mingw() = default;
  Toolchain_mingw(const Toolchain_mingw &) = delete;

private:
  void Initialize();
  std::optional<ToolchainCompilerInfo>
  GetToolchainInfo(const ToolchainExecutables &executables) const override;
};

} // namespace buildcc

#endif
