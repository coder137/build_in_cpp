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

#ifndef TOOLCHAINS_TOOLCHAIN_GCC_H_
#define TOOLCHAINS_TOOLCHAIN_GCC_H_

#include "toolchain/toolchain.h"

namespace buildcc {

/**
 * @brief Generic GCC Toolchain <br>
 * id = ToolchainId::Gcc <br>
 * name = "gcc" <br>
 * asm_compiler = "as" <br>
 * c_compiler = "gcc" <br>
 * cpp_compiler = "g++" <br>
 * archiver = "ar" <br>
 * linker = "ld" <br>
 */
class Toolchain_gcc : public Toolchain {
public:
  // Run time basic constructor
  Toolchain_gcc(const std::string &name = "gcc",
                const env::optional<ToolchainExecutables> &op_executables = {},
                const env::optional<ToolchainConfig> &op_config = {})
      : Toolchain(ToolchainId::Gcc, name,
                  op_executables.value_or(
                      ToolchainExecutables("as", "gcc", "g++", "ar", "ld")),
                  op_config.value_or(ToolchainConfig())) {
    Initialize();
  }

  virtual ~Toolchain_gcc() = default;
  Toolchain_gcc(const Toolchain_gcc &) = delete;

private:
  void Initialize();
};

} // namespace buildcc

#endif
