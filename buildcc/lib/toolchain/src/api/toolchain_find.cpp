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

#include "toolchain/api/toolchain_find.h"

#include "env/assert_fatal.h"
#include "env/host_os.h"
#include "env/host_os_util.h"
#include "env/util.h"

#include "toolchain/toolchain.h"

namespace {

std::vector<std::string> ParseEnvVarToPaths(const std::string &env_var) {
  const char *path_env = getenv(env_var.c_str());
  buildcc::env::assert_fatal(
      path_env != nullptr,
      fmt::format("Environment variable '{}' not present", env_var));

  constexpr const char *os_env_delim = buildcc::env::get_os_envvar_delim();
  buildcc::env::assert_fatal<os_env_delim != nullptr>("OS not supported");
  std::vector<std::string> paths =
      buildcc::env::split(path_env, os_env_delim[0]);

  return paths;
}

bool ContainsToolchainBinaries(const fs::directory_iterator &directory_iterator,
                               const buildcc::ToolchainExecutables &binaries) {
  std::unordered_set<std::string> bins({binaries.assembler, binaries.c_compiler,
                                        binaries.cpp_compiler,
                                        binaries.archiver, binaries.linker});
  std::error_code ec;
  for (const auto &dir_iter : directory_iterator) {
    bool is_regular_file = dir_iter.is_regular_file(ec);
    if (!is_regular_file || ec) {
      continue;
    }
    const auto &filename_without_ext = dir_iter.path().stem().string();
    // NOTE, Must match the entire filename
    bins.erase(filename_without_ext);
  }
  return bins.empty();
}

} // namespace

namespace buildcc {

template <typename T>
std::vector<fs::path>
ToolchainFind<T>::Find(const ToolchainFindConfig &config) const {
  // Initialization
  const T &t = static_cast<const T &>(*this);
  std::vector<fs::path> found_toolchains;
  fs_unordered_set absolute_search_paths(config.absolute_search_paths);

  // Parse config envs and add it to absolute search paths
  for (const std::string &env_var : config.env_vars) {
    std::vector<std::string> paths = ParseEnvVarToPaths(env_var);
    absolute_search_paths.insert(paths.begin(), paths.end());
  }

  // Over the absolute search paths
  // - Check if directory exists
  // - Iterate over directory
  // - Find ALL Toolchain binaries in ONE directory
  // - If matched, store that path
  for (const auto &search_path : absolute_search_paths) {
    if (!fs::exists(search_path)) {
      continue;
    }

    std::error_code ec;
    auto directory_iterator = fs::directory_iterator(search_path, ec);
    if (ec) {
      continue;
    }

    bool toolchains_matched = ContainsToolchainBinaries(
        directory_iterator, t.GetToolchainExecutables());
    if (toolchains_matched) {
      found_toolchains.push_back(search_path);
    }
  }

  return found_toolchains;
}

template class ToolchainFind<Toolchain>;

} // namespace buildcc
