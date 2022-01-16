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

#include "env/logging.h"

#include "fmt/format.h"

namespace fs = std::filesystem;

namespace buildcc {

/**
 * @param absolute_search_paths absolute_search_paths expect directories that
 * are iterated for exact toolchain matches
 * @param env_vars env_vars contain paths that are seperated by OS delimiter.
 * These are converted to paths and searched similarly to absolute_search_paths
 * NOTE: env_vars must contain single absolute paths or multiple absolute paths
 * seperated by OS delimiter
 *
 * Example: [Windows]   "absolute_path_1;absolute_path_2;..."
 * Example: [Linux]     "absolute_path_1:absolute_path_2:..."
 */
struct VerifyToolchainConfig {
  std::vector<std::string> absolute_search_paths;
  std::vector<std::string> env_vars{"PATH"};

  std::optional<std::string> compiler_version;
  std::optional<std::string> target_arch;

  // Updates the toolchain with absolute paths once verified
  // If multiple toolchains are found, uses the first in the list
  bool update{true};
};

/**
 * @param path
 * @param compiler_version
 * @param target_arch
 */
struct VerifiedToolchain {
  fs::path path;
  std::string compiler_version;
  std::string target_arch;
  // TODO, Add more here as needed

  std::string ToString() const { return fmt::format("{}", *this); }
};

template <typename T> class ToolchainVerify {
public:
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
  std::vector<VerifiedToolchain>
  Verify(const VerifyToolchainConfig &config = VerifyToolchainConfig());

protected:
  VerifiedToolchain verified_toolchain_;
};

} // namespace buildcc

constexpr const char *const kVerifiedToolchainFormat = R"({{
  "path": "{}",
  "compiler_version": "{}",
  "target_arch": "{}"
}})";

template <>
struct fmt::formatter<buildcc::VerifiedToolchain> : formatter<std::string> {
  template <typename FormatContext>
  auto format(const buildcc::VerifiedToolchain &vt, FormatContext &ctx) {
    std::string verified_toolchain_info =
        fmt::format(kVerifiedToolchainFormat, vt.path.string(),
                    vt.compiler_version, vt.target_arch);
    return formatter<std::string>::format(verified_toolchain_info, ctx);
  }
};

#endif
