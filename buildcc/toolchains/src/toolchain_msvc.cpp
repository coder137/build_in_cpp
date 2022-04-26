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

#include "toolchains/toolchain_msvc.h"

namespace {

constexpr const char *const kMsvcObjExt = ".obj";
constexpr const char *const kMsvcPchHeaderExt = ".h";
constexpr const char *const kMsvcPchCompileExt = ".pch";
constexpr const char *const kMsvcPrefixIncludeDir = "/I";
constexpr const char *const kMsvcPrefixLibDir = "/LIBPATH:";

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

} // namespace

namespace buildcc {

void Toolchain_msvc::UpdateConfig(ToolchainConfig &config) {
  config.obj_ext = kMsvcObjExt;
  config.pch_header_ext = kMsvcPchHeaderExt;
  config.pch_compile_ext = kMsvcPchCompileExt;
  config.prefix_include_dir = kMsvcPrefixIncludeDir;
  config.prefix_lib_dir = kMsvcPrefixLibDir;
}

std::optional<ToolchainCompilerInfo> Toolchain_msvc::GetToolchainInfo(
    const ToolchainExecutables &executables) const {
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

} // namespace buildcc
