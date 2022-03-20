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

#ifndef TOOLCHAIN_TOOLCHAIN_FIND_H_
#define TOOLCHAIN_TOOLCHAIN_FIND_H_

#include <filesystem>
#include <string>
#include <unordered_set>
#include <vector>

#include "schema/path.h"

namespace fs = std::filesystem;

namespace buildcc {

/**
 * @brief Configure the behaviour of Toolchain::Find API. By default searches
 * the directories mentioned in the ENV{PATH} variable to find the toolchain.
 * @param absolute_search_paths absolute_search_paths expect directories that
 * are iterated for exact toolchain matches
 * @param env_vars env_vars contain paths that are seperated by OS delimiter.
 * These are converted to paths and searched similarly to absolute_search_paths
 * <br>
 */
struct ToolchainFindConfig {
  ToolchainFindConfig(
      const std::unordered_set<std::string> &env_vars = {"PATH"},
      const fs_unordered_set &absolute_search_paths = {})
      : env_vars(env_vars), absolute_search_paths(absolute_search_paths) {}

  std::unordered_set<std::string> env_vars;
  fs_unordered_set absolute_search_paths;
};

template <typename T> class ToolchainFind {
public:
  std::vector<fs::path>
  Find(const ToolchainFindConfig &config = ToolchainFindConfig());
};

} // namespace buildcc

#endif
