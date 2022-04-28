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

} // namespace

namespace buildcc {

template <typename T>
ToolchainCompilerInfo
ToolchainVerify<T>::Verify(const ToolchainFindConfig &config) {
  T &t = static_cast<T &>(*this);
  std::vector<fs::path> toolchain_paths = t.Find(config);
  env::assert_fatal(!toolchain_paths.empty(), "No toolchains found");

  ToolchainExecutables exes =
      CreateToolchainExecutables(toolchain_paths[0], t.executables_);
  auto op_toolchain_compiler_info = t.GetToolchainInfo(exes);
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
template <typename T>
std::optional<ToolchainCompilerInfo> ToolchainVerify<T>::GetToolchainInfo(
    const ToolchainExecutables &executables) const {
  const auto &cb = GetToolchainInfoFunc();
  if (cb) {
    return cb(executables);
  }
  env::log_critical(__FUNCTION__,
                    "GetToolchainInfo virtual function not implemented");
  return {};
}

template class ToolchainVerify<Toolchain>;

} // namespace buildcc
