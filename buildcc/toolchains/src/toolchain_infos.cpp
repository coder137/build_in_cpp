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

#include "toolchains/toolchain_infos.h"

#include "env/assert_fatal.h"
#include "env/command.h"
#include "env/logging.h"

namespace {

// GCC

std::optional<std::string>
GetGccCompilerVersion(const buildcc::env::Command &command) {
  std::vector<std::string> stdout_data;
  bool executed = buildcc::env::Command::Execute(
      command.Construct("{compiler} -dumpversion"), {}, &stdout_data);
  if (!executed || stdout_data.empty()) {
    return {};
  }
  return stdout_data[0];
}

std::optional<std::string>
GetGccTargetArchitecture(const buildcc::env::Command &command) {
  std::vector<std::string> stdout_data;
  bool executed = buildcc::env::Command::Execute(
      command.Construct("{compiler} -dumpmachine"), {}, &stdout_data);
  if (!executed || stdout_data.empty()) {
    return {};
  }
  return stdout_data[0];
}

std::optional<buildcc::ToolchainCompilerInfo>
GetGccToolchainInfo(const buildcc::ToolchainExecutables &executables) {
  buildcc::env::Command command;
  command.AddDefaultArgument("compiler", executables.cpp_compiler);

  auto op_compiler_version = GetGccCompilerVersion(command);
  auto op_target_arch = GetGccTargetArchitecture(command);
  if (!op_compiler_version.has_value() || !op_target_arch.has_value()) {
    return {};
  }

  buildcc::ToolchainCompilerInfo compiler_info;
  compiler_info.compiler_version = op_compiler_version.value();
  compiler_info.target_arch = op_target_arch.value();
  return compiler_info;
}

// MSVC

std::optional<std::string> GetMsvcCompilerVersion() {
  const char *vscmd_version = getenv("VSCMD_VER");
  if (vscmd_version == nullptr) {
    return {};
  }
  return vscmd_version;
}

std::optional<std::string> GetMsvcTargetArchitecture() {
  // DONE, Read `VSCMD_ARG_HOST_ARCH` from env path
  // DONE, Read `VSCMD_ARG_TGT_ARCH` from env path
  const char *vs_host_arch = getenv("VSCMD_ARG_HOST_ARCH");
  const char *vs_target_arch = getenv("VSCMD_ARG_TGT_ARCH");
  if (vs_host_arch == nullptr || vs_target_arch == nullptr) {
    return {};
  }

  // DONE, Concat them
  return fmt::format("{}_{}", vs_host_arch, vs_target_arch);
}

std::optional<buildcc::ToolchainCompilerInfo>
GetMsvcToolchainInfo(const buildcc::ToolchainExecutables &executables) {
  (void)executables;
  auto op_compiler_version = GetMsvcCompilerVersion();
  auto op_target_arch = GetMsvcTargetArchitecture();
  if (!op_compiler_version.has_value() || !op_target_arch.has_value()) {
    return {};
  }

  buildcc::ToolchainCompilerInfo compiler_info;
  compiler_info.compiler_version = op_compiler_version.value();
  compiler_info.target_arch = op_target_arch.value();
  return compiler_info;
}

//

std::optional<buildcc::ToolchainCompilerInfo>
GetErrorToolchainInfo(const buildcc::ToolchainExecutables &executables) {
  (void)executables;
  buildcc::env::log_critical(__FUNCTION__,
                             "ToolchainInfo does not exist for particular "
                             "ToolchainId. Supply your own through 3 methods.");
  return {};
}

} // namespace

namespace buildcc {

// TODO, Shift this to toolchain.h
// Create a global_toolchain.h file which manages global toolchain state
std::unordered_map<ToolchainId, ToolchainInfoFunc>
    GlobalToolchainInfo::global_toolchain_info_func_{
        {ToolchainId::Gcc, GetGccToolchainInfo},
        {ToolchainId::MinGW, GetGccToolchainInfo},
        {ToolchainId::Clang, GetGccToolchainInfo},
        {ToolchainId::Msvc, GetMsvcToolchainInfo},
        {ToolchainId::Custom, GetErrorToolchainInfo},
        {ToolchainId::Undefined, GetErrorToolchainInfo},
    };

const ToolchainInfoFunc &GlobalToolchainInfo::Get(ToolchainId id) {
  env::assert_fatal(global_toolchain_info_func_.find(id) !=
                        global_toolchain_info_func_.end(),
                    "Invalid ToolchainId");
  return global_toolchain_info_func_[id];
}

} // namespace buildcc
