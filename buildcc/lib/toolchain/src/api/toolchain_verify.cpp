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

#include "toolchain/api/toolchain_verify.h"

#include <optional>
#include <unordered_set>
#include <vector>

#include <iostream>

#include "schema/path.h"

#include "env/assert_fatal.h"
#include "env/host_os.h"
#include "env/host_os_util.h"
#include "env/util.h"

#include "env/command.h"

#include "toolchain/toolchain.h"

namespace {

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
  if (!executed && stdout_data.empty()) {
    return {};
  }
  return stdout_data[0];
}

std::optional<buildcc::ToolchainCompilerInfo>
GccVerificationFunc(const buildcc::ToolchainExecutables &executables) {
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
MsvcVerificationFunc(const buildcc::ToolchainExecutables &executables) {
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

buildcc::ToolchainExecutables CreateToolchainExecutables(
    const fs::path &absolute_path,
    const buildcc::ToolchainExecutables &current_executables) {
  constexpr const char *const executable_ext =
      buildcc::env::get_os_executable_extension();
  buildcc::env::assert_fatal<executable_ext != nullptr>(
      "Host executable extension not supported");

  std::string assembler_path =
      (absolute_path /
       fmt::format("{}{}", current_executables.assembler, executable_ext))
          .string();
  std::string c_compiler_path =
      (absolute_path /
       fmt::format("{}{}", current_executables.c_compiler, executable_ext))
          .string();
  std::string cpp_compiler_path =
      (absolute_path /
       fmt::format("{}{}", current_executables.cpp_compiler, executable_ext))
          .string();
  std::string archiver_path =
      (absolute_path /
       fmt::format("{}{}", current_executables.archiver, executable_ext))
          .string();
  std::string linker_path =
      (absolute_path /
       fmt::format("{}{}", current_executables.linker, executable_ext))
          .string();

  return buildcc::ToolchainExecutables(assembler_path, c_compiler_path,
                                       cpp_compiler_path, archiver_path,
                                       linker_path);
}

std::string
GetToolchainVerifyIdentifier(buildcc::ToolchainId id,
                             const std::optional<std::string> &op_identifier) {
  std::string identifier;
  switch (id) {
  case buildcc::ToolchainId::Custom:
    buildcc::env::assert_fatal(
        op_identifier.has_value(),
        "Requires verification_identifier value in ToolchainVerifyConfig");
    identifier = op_identifier.value();
    break;
  case buildcc::ToolchainId::Gcc:
  case buildcc::ToolchainId::Msvc:
  case buildcc::ToolchainId::Clang:
  case buildcc::ToolchainId::MinGW:
    identifier = fmt::format("{}", id);
    break;
  case buildcc::ToolchainId::Undefined:
  default:
    buildcc::env::assert_fatal<false>(
        "Undefined toolchain. Use valid ToolchainId");
    break;
  }
  return identifier;
}

} // namespace

namespace buildcc {

template <typename T>
void ToolchainVerify<T>::AddVerificationFunc(
    ToolchainId id, const ToolchainVerificationFunc &verification_func,
    const std::optional<std::string> &op_identifier) {
  std::string identifier;
  switch (id) {
  case ToolchainId::Gcc:
  case ToolchainId::Msvc:
  case ToolchainId::MinGW:
  case ToolchainId::Clang:
    identifier = fmt::format("{}", id);
    break;
  case ToolchainId::Custom:
    env::assert_fatal(op_identifier.has_value(),
                      "Requires optional identifier parameter when "
                      "ToolchainId::Custom is defined");
    identifier = op_identifier.value();
    break;
  default:
    env::assert_fatal<false>("Invalid ToolchainId parameter");
    break;
  }

  env::assert_fatal(
      ToolchainVerify::GetStatic().count(identifier) == 0,
      fmt::format("Already registered VerificationFunction for identifier '{}'",
                  identifier));
  ToolchainVerify::GetStatic()[identifier] = verification_func;
}

template <typename T>
ToolchainCompilerInfo
ToolchainVerify<T>::Verify(const ToolchainVerifyConfig &config) {
  T &t = static_cast<T &>(*this);
  std::vector<fs::path> toolchain_paths = t.Find(config);
  env::assert_fatal(!toolchain_paths.empty(), "No toolchains found");

  ToolchainExecutables exes =
      CreateToolchainExecutables(toolchain_paths[0], t.executables_);

  std::string toolchain_id_identifier =
      GetToolchainVerifyIdentifier(t.GetId(), config.verification_identifier);

  const auto &verification_func =
      T::GetVerificationFunc(toolchain_id_identifier);
  auto op_toolchain_compiler_info = verification_func(exes);
  env::assert_fatal(op_toolchain_compiler_info.has_value(),
                    "Could not verify toolchain");

  ToolchainCompilerInfo toolchain_compiler_info =
      op_toolchain_compiler_info.value();
  toolchain_compiler_info.path = toolchain_paths[0];

  // Update the compilers
  t.executables_ = exes;
  return toolchain_compiler_info;
}

// PRIVATE
template <typename T> void ToolchainVerify<T>::Initialize() {
  static bool do_once = true;

  if (do_once) {
    do_once = false;
    AddVerificationFunc(ToolchainId::Gcc, GccVerificationFunc);
    AddVerificationFunc(ToolchainId::Msvc, MsvcVerificationFunc);
    AddVerificationFunc(ToolchainId::Clang, GccVerificationFunc);
    AddVerificationFunc(ToolchainId::MinGW, GccVerificationFunc);
  }
}

template <typename T>
const ToolchainVerificationFunc &
ToolchainVerify<T>::GetVerificationFunc(const std::string &identifier) {
  const auto &verification_map = T::GetStatic();
  env::assert_fatal(verification_map.count(identifier) == 1,
                    "Add verification for custom toolchain through "
                    "Toolchain::AddVerificationFunc API");
  return verification_map.at(identifier);
}

template class ToolchainVerify<Toolchain>;

} // namespace buildcc
