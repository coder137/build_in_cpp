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

#include "plugins/buildcc_find.h"

#include <cstdlib>
#include <cstring>
#include <regex>

#include "env/logging.h"

#include "target/base/path.h"

namespace {
constexpr const char *const kEnvVarNotFound =
    "{} environment variable not found";
constexpr const char *const kOsNotSupported =
    "Search functionality does not support this operating system. Raise an "
    "issue at https://github.com/coder137/build_in_cpp outlining your OS and "
    "usecase";

// Windows
constexpr const char *const kWinEnvDelim = ";";

// Linux
constexpr const char *const kLinuxEnvDelim = ":";

// TODO, Add Environment Variable delimiters for more operating systems
// ; for windows
// : for linux
constexpr char const *GetEnvVarDelim() {
  if constexpr (buildcc::env::is_win()) {
    return kWinEnvDelim;
  } else if constexpr (buildcc::env::is_linux()) {
    return kLinuxEnvDelim;
  }

  return nullptr;
}

std::vector<fs::path> SplitEnv(const char *env_ptr, const char *delim) {
  std::vector<fs::path> env_paths;

  std::string temp{env_ptr};
  char *path = std::strtok(temp.data(), delim);
  while (path != nullptr) {
    env_paths.push_back(
        buildcc::internal::Path::CreateNewPath(path).GetPathname());
    path = std::strtok(nullptr, delim);
  }

  return env_paths;
}

std::vector<fs::path> SearchEnv(const std::string &host_env_var,
                                const std::string &regex) {
  char *path_ptr = std::getenv(host_env_var.c_str());
  if (path_ptr == nullptr) {
    buildcc::env::log_critical(__FUNCTION__,
                               fmt::format(kEnvVarNotFound, host_env_var));
    return {};
  }

  std::vector<fs::path> env_paths;
  constexpr const char *const kDelim = GetEnvVarDelim();
  if constexpr (kDelim == nullptr) {
    buildcc::env::log_critical(__FUNCTION__, kOsNotSupported);
    return {};
  }
  env_paths = SplitEnv(path_ptr, kDelim);

  // DONE, Construct a directory iterator
  // Only take the files
  std::vector<fs::path> matches;
  for (const auto &env_p : env_paths) {
    std::error_code errcode;
    const auto dir_iter = fs::directory_iterator(env_p, errcode);
    if (errcode) {
      buildcc::env::log_critical(env_p.string(), errcode.message());
      continue;
    }

    for (const auto &dir_entry : dir_iter) {
      if (!dir_entry.is_regular_file()) {
        continue;
      }

      // Compare name_ with filename
      std::string filename = dir_entry.path().filename().string();
      bool match = std::regex_match(filename, std::regex(regex));
      if (match) {
        matches.push_back(dir_entry.path());
      }
    }
  }

  return matches;
}

} // namespace

namespace buildcc::plugin {

// Public

bool BuildccFind::Search() {
  switch (type_) {
  case Type::HostExecutable:
    return SearchHostExecutable();
    break;
  case Type::BuildccLibrary:
  case Type::BuildccPlugin:
  default:
    // TODO, Add this functionality
    break;
  }

  return false;
}

// Private

bool BuildccFind::SearchHostExecutable() {
  for (const auto &ev : host_env_vars_) {
    std::vector<fs::path> matches = SearchEnv(ev, regex_);
    target_matches_.insert(target_matches_.end(), matches.begin(),
                           matches.end());
  }

  return !target_matches_.empty();
}

} // namespace buildcc::plugin
