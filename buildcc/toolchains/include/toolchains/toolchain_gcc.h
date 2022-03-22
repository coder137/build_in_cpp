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

constexpr const char *const kGccObjExt = ".o";
constexpr const char *const kGccPchHeaderExt = ".h";
constexpr const char *const kGccPchCompileExt = ".gch";
constexpr const char *const kGccPrefixIncludeDir = "-I";
constexpr const char *const kGccPrefixLibDir = "-L";

inline std::optional<std::string>
GetGccCompilerVersion(const buildcc::env::Command &command) {
  std::vector<std::string> stdout_data;
  bool executed = buildcc::env::Command::Execute(
      command.Construct("{compiler} -dumpversion"), {}, &stdout_data);
  if (!executed || stdout_data.empty()) {
    return {};
  }
  return stdout_data[0];
}

inline std::optional<std::string>
GetGccTargetArchitecture(const buildcc::env::Command &command) {
  std::vector<std::string> stdout_data;
  bool executed = buildcc::env::Command::Execute(
      command.Construct("{compiler} -dumpmachine"), {}, &stdout_data);
  if (!executed && stdout_data.empty()) {
    return {};
  }
  return stdout_data[0];
}

inline std::optional<ToolchainCompilerInfo>
VerifyGccSelectedToolchainPath(const ToolchainExecutables &executables) {
  env::Command command;
  command.AddDefaultArgument("compiler", executables.cpp_compiler);

  auto op_compiler_version = GetGccCompilerVersion(command);
  auto op_target_arch = GetGccTargetArchitecture(command);
  if (!op_compiler_version.has_value() || !op_target_arch.has_value()) {
    return {};
  }

  ToolchainCompilerInfo compiler_info;
  compiler_info.compiler_version = op_compiler_version.value();
  compiler_info.target_arch = op_target_arch.value();
  return compiler_info;
}

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
  Toolchain_gcc()
      : Toolchain(ToolchainId::Gcc, "gcc", "as", "gcc", "g++", "ar", "ld") {}
  Toolchain_gcc(const Toolchain_gcc &gcc) = delete;

private:
  void UpdateConfig(ToolchainConfig &config) override {
    config.obj_ext = kGccObjExt;
    config.pch_header_ext = kGccPchHeaderExt;
    config.pch_compile_ext = kGccPchCompileExt;
    config.prefix_include_dir = kGccPrefixIncludeDir;
    config.prefix_lib_dir = kGccPrefixLibDir;
  }

  virtual std::optional<ToolchainCompilerInfo>
  VerifySelectedToolchainPath(const ToolchainExecutables &executables) const {
    return VerifyGccSelectedToolchainPath(executables);
  }
};

} // namespace buildcc

#endif
