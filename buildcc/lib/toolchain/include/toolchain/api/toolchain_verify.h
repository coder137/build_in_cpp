/*
 * Copyright 2021 Niket Naidu. All rights reserved.
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
#include <vector>

#include "env/logging.h"

#include "fmt/format.h"

namespace fs = std::filesystem;

namespace buildcc::base {

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

  void Print() const {
    env::log_info(
        __FUNCTION__,
        fmt::format("Path: {}, Compiler Version: {}, Target Triple Arch: {}",
                    path.string(), compiler_version, target_arch));
  }
};

template <typename T> class ToolchainVerify {
public:
  std::vector<VerifiedToolchain>
  Verify(const VerifyToolchainConfig &config = VerifyToolchainConfig());
};

} // namespace buildcc::base

namespace buildcc {

typedef base::VerifyToolchainConfig VerifyToolchainConfig;
typedef base::VerifiedToolchain VerifiedToolchain;

} // namespace buildcc

#endif
