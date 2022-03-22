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

constexpr const char *const kMingwObjExt = ".o";
constexpr const char *const kMingwPchHeaderExt = ".h";
constexpr const char *const kMingwPchCompileExt = ".gch";
constexpr const char *const kMingwPrefixIncludeDir = "-I";
constexpr const char *const kMingwPrefixLibDir = "-L";

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
class Toolchain_mingw : public BaseToolchain {
public:
  Toolchain_mingw()
      : Toolchain(ToolchainId::MinGW, "gcc", "as", "gcc", "g++", "ar", "ld") {}
  Toolchain_mingw(const Toolchain_mingw &) = delete;

private:
  void UpdateConfig(ToolchainConfig &config) {
    config.obj_ext = kMingwObjExt;
    config.pch_header_ext = kMingwPchHeaderExt;
    config.pch_compile_ext = kMingwPchCompileExt;
    config.prefix_include_dir = kMingwPrefixIncludeDir;
    config.prefix_lib_dir = kMingwPrefixLibDir;
  }
  virtual std::optional<ToolchainCompilerInfo>
  VerifySelectedToolchainPath(const ToolchainBinaries &binaries) const {
    return VerifyGccSelectedToolchainPath(binaries);
  }
};

} // namespace buildcc

#endif
