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
constexpr const char *const kGccObjExt = ".o";
constexpr const char *const kGccPchHeaderExt = ".h";
constexpr const char *const kGccPchCompileExt = ".gch";
constexpr const char *const kGccPrefixIncludeDir = "-I";
constexpr const char *const kGccPrefixLibDir = "-L";
buildcc::ToolchainConfig GetGccToolchainConfig() {
  buildcc::ToolchainConfig config;
  config.obj_ext = kGccObjExt;
  config.pch_header_ext = kGccPchHeaderExt;
  config.pch_compile_ext = kGccPchCompileExt;
  config.prefix_include_dir = kGccPrefixIncludeDir;
  config.prefix_lib_dir = kGccPrefixLibDir;
  return config;
}

buildcc::env::optional<std::string>
GetGccCompilerVersion(const buildcc::env::Command &command) {
  std::vector<std::string> stdout_data;
  bool executed = buildcc::env::Command::Execute(
      command.Construct("{compiler} -dumpversion"), {}, &stdout_data);
  if (!executed || stdout_data.empty()) {
    return {};
  }
  return stdout_data[0];
}

buildcc::env::optional<std::string>
GetGccTargetArchitecture(const buildcc::env::Command &command) {
  std::vector<std::string> stdout_data;
  bool executed = buildcc::env::Command::Execute(
      command.Construct("{compiler} -dumpmachine"), {}, &stdout_data);
  if (!executed || stdout_data.empty()) {
    return {};
  }
  return stdout_data[0];
}

buildcc::env::optional<buildcc::ToolchainCompilerInfo>
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

constexpr const char *const kMsvcObjExt = ".obj";
constexpr const char *const kMsvcPchHeaderExt = ".h";
constexpr const char *const kMsvcPchCompileExt = ".pch";
constexpr const char *const kMsvcPrefixIncludeDir = "/I";
constexpr const char *const kMsvcPrefixLibDir = "/LIBPATH:";
buildcc::ToolchainConfig GetMsvcToolchainConfig() {
  buildcc::ToolchainConfig config;
  config.obj_ext = kMsvcObjExt;
  config.pch_header_ext = kMsvcPchHeaderExt;
  config.pch_compile_ext = kMsvcPchCompileExt;
  config.prefix_include_dir = kMsvcPrefixIncludeDir;
  config.prefix_lib_dir = kMsvcPrefixLibDir;
  return config;
}

buildcc::env::optional<std::string> GetMsvcCompilerVersion() {
  const char *vscmd_version = getenv("VSCMD_VER");
  if (vscmd_version == nullptr) {
    return {};
  }
  return vscmd_version;
}

buildcc::env::optional<std::string> GetMsvcTargetArchitecture() {
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

buildcc::env::optional<buildcc::ToolchainCompilerInfo>
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

buildcc::env::optional<buildcc::ToolchainCompilerInfo>
GetErrorToolchainInfo(const buildcc::ToolchainExecutables &executables) {
  (void)executables;
  buildcc::env::log_critical(__FUNCTION__,
                             "ToolchainInfo does not exist for particular "
                             "ToolchainId. Supply your own through "
                             "Toolchain::SetToolchainInfoCb method.");
  return {};
}

} // namespace

namespace buildcc {

std::unordered_map<ToolchainId, GlobalToolchainMetadata::ToolchainMetadata>
    GlobalToolchainMetadata::global_toolchain_metadata_{
        {ToolchainId::Gcc,
         ToolchainMetadata(GetGccToolchainConfig(), GetGccToolchainInfo)},
        {ToolchainId::MinGW,
         ToolchainMetadata(GetGccToolchainConfig(), GetGccToolchainInfo)},
        {ToolchainId::Clang,
         ToolchainMetadata(GetGccToolchainConfig(), GetGccToolchainInfo)},
        {ToolchainId::Msvc,
         ToolchainMetadata(GetMsvcToolchainConfig(), GetMsvcToolchainInfo)},
        {ToolchainId::Custom,
         ToolchainMetadata(ToolchainConfig(), GetErrorToolchainInfo)},
        {ToolchainId::Undefined,
         ToolchainMetadata(ToolchainConfig(), GetErrorToolchainInfo)},
    };

const ToolchainConfig &GlobalToolchainMetadata::GetConfig(ToolchainId id) {
  Expect(id);
  return Get(id).config_;
}
const ToolchainInfoCb &GlobalToolchainMetadata::GetInfoCb(ToolchainId id) {
  Expect(id);
  return Get(id).cb_;
}

// PRIVATE
void GlobalToolchainMetadata::Expect(ToolchainId id) {
  env::assert_fatal(global_toolchain_metadata_.find(id) !=
                        global_toolchain_metadata_.end(),
                    "Invalid ToolchainId");
}
const GlobalToolchainMetadata::ToolchainMetadata &
GlobalToolchainMetadata::Get(ToolchainId id) {
  return global_toolchain_metadata_.at(id);
}

} // namespace buildcc
