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

#ifndef TOOLCHAIN_TOOLCHAIN_VERIFY_H_
#define TOOLCHAIN_TOOLCHAIN_VERIFY_H_

#include <filesystem>
#include <optional>
#include <vector>

#include "fmt/format.h"

#include "env/logging.h"

#include "toolchain/common/toolchain_executables.h"
#include "toolchain/common/toolchain_id.h"

#include "toolchain/api/toolchain_find.h"

namespace fs = std::filesystem;

namespace buildcc {

/**
 * @brief Verified Toolchain information
 * @param path Absolute host path where ALL the toolchain executables are found
 * <br>
 * NOTE: All the Toolchain executables must be found in a single folder. <br>
 * @param compiler_version Compiler version of the verified toolchain
 * @param target_arch Target architecture of the verified toolchain
 */
struct ToolchainCompilerInfo {
  std::string ToString() const { return fmt::format("{}", *this); }

  fs::path path;
  std::string compiler_version;
  std::string target_arch;
};

// clang-format off
typedef std::function<std::optional<ToolchainCompilerInfo>(const ToolchainExecutables &)> ToolchainInfoFunc;
// clang-format on

template <typename T> class ToolchainVerify {
public:
  ToolchainVerify() = default;

  /**
   * @brief Verify your toolchain executables by searching your operating system
   * paths
   * Only add the verified path IF all toolchain executables are matched
   *
   * @param config Search paths to find toolchains
   * @return std::vector<VerifiedToolchain> Operating system can contain
   * multiple toolchains of similar names with different versions. Collect all
   * of them
   */
  ToolchainCompilerInfo
  Verify(const ToolchainFindConfig &config = ToolchainFindConfig());

  /**
   * @brief Set ToolchainInfo callback for run time objects
   */
  void SetToolchainInfoFunc(const ToolchainInfoFunc &cb) { info_func_ = cb; }
  const ToolchainInfoFunc &GetToolchainInfoFunc() const { return info_func_; }

private:
  /**
   * @brief ToolchainInfo callback for compile time polymorphic objects
   */
  virtual std::optional<ToolchainCompilerInfo>
  GetToolchainInfo(const ToolchainExecutables &executables) const;

private:
  ToolchainInfoFunc info_func_;
};

} // namespace buildcc

constexpr const char *const kVerifiedToolchainFormat = R"({{
  "path": "{}",
  "compiler_version": "{}",
  "target_arch": "{}"
}})";

template <>
struct fmt::formatter<buildcc::ToolchainCompilerInfo> : formatter<std::string> {
  template <typename FormatContext>
  auto format(const buildcc::ToolchainCompilerInfo &vt, FormatContext &ctx) {
    std::string verified_toolchain_info =
        fmt::format(kVerifiedToolchainFormat, vt.path.string(),
                    vt.compiler_version, vt.target_arch);
    return formatter<std::string>::format(verified_toolchain_info, ctx);
  }
};

#endif
