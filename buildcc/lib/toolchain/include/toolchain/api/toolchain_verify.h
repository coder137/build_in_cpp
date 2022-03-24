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

struct ToolchainVerifyConfig : public ToolchainFindConfig {
  std::optional<std::string> verification_identifier;
};

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

typedef std::function<std::optional<ToolchainCompilerInfo>(
    const ToolchainExecutables &)>
    ToolchainVerificationFunc;

template <typename T> class ToolchainVerify {
public:
  ToolchainVerify() { Initialize(); }

  /**
   * @brief
   *
   * @param id
   * @param verification_func
   * @param identifier Only read when ToolchainId::Custom is passed in
   */
  static void
  AddVerificationFunc(ToolchainId id,
                      const ToolchainVerificationFunc &verification_func,
                      const std::optional<std::string> &op_identifier = {});

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
  Verify(const ToolchainVerifyConfig &config = ToolchainVerifyConfig());

protected:
  ToolchainCompilerInfo verified_toolchain_;

private:
  void Initialize();
  static const ToolchainVerificationFunc &
  GetVerificationFunc(const std::string &identifier);
  static std::unordered_map<std::string, ToolchainVerificationFunc> &
  GetStatic() {
    static std::unordered_map<std::string, ToolchainVerificationFunc>
        verification_func_map;
    return verification_func_map;
  }
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
