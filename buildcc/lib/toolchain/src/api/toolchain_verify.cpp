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

#include "toolchain/toolchain.h"

#include "env/assert_fatal.h"
#include "env/host_os.h"
#include "env/host_os_util.h"
#include "env/util.h"

#include "env/command.h"

namespace buildcc {

template <typename T>
ToolchainCompilerInfo
ToolchainVerify<T>::Verify(const ToolchainFindConfig &config) {
  T &t = static_cast<T &>(*this);
  std::vector<fs::path> toolchain_paths = t.Find(config);
  env::assert_fatal(!toolchain_paths.empty(), "No toolchains found");
  std::optional<ToolchainCompilerInfo> op_toolchain_compiler_info =
      t.VerifySelectedToolchainPath(toolchain_paths[0], config);
  env::assert_fatal(op_toolchain_compiler_info.has_value(),
                    "Could not verify toolchain");

  ToolchainCompilerInfo toolchain_compiler_info =
      op_toolchain_compiler_info.value();
  constexpr const char *const executable_ext =
      env::get_os_executable_extension();
  env::assert_fatal<executable_ext != nullptr>(
      "Host executable extension not supported");

  // Update the compilers
  t.binaries_.assembler =
      (toolchain_compiler_info.path /
       fmt::format("{}{}", t.binaries_.assembler, executable_ext))
          .make_preferred()
          .string();
  t.binaries_.c_compiler =
      (toolchain_compiler_info.path /
       fmt::format("{}{}", t.binaries_.c_compiler, executable_ext))
          .make_preferred()
          .string();
  t.binaries_.cpp_compiler =
      (toolchain_compiler_info.path /
       fmt::format("{}{}", t.binaries_.cpp_compiler, executable_ext))
          .make_preferred()
          .string();
  t.binaries_.archiver =
      (toolchain_compiler_info.path /
       fmt::format("{}{}", t.binaries_.archiver, executable_ext))
          .make_preferred()
          .string();
  t.binaries_.linker = (toolchain_compiler_info.path /
                        fmt::format("{}{}", t.binaries_.linker, executable_ext))
                           .make_preferred()
                           .string();
  return toolchain_compiler_info;
}

template class ToolchainVerify<Toolchain>;

} // namespace buildcc
