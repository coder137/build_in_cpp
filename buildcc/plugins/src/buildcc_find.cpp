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

#include "plugins/buildcc_find.h"

#include <cstdlib>
#include <cstring>
#include <regex>

#include "env/host_os_util.h"
#include "env/logging.h"
#include "env/util.h"

#include "schema/path.h"

namespace {
constexpr const char *const kEnvVarNotFound =
    "{} environment variable not found";
constexpr const char *const kOsNotSupported =
    "Search functionality does not support this operating system. Raise an "
    "issue at https://github.com/coder137/build_in_cpp outlining your OS and "
    "usecase";

std::vector<fs::path> SearchEnv(const std::string &host_env_var,
                                const std::string &regex) {
  char *path_ptr = std::getenv(host_env_var.c_str());
  if (path_ptr == nullptr) {
    buildcc::env::log_critical(__FUNCTION__,
                               fmt::format(kEnvVarNotFound, host_env_var));
    return {};
  }

  constexpr const char *const kDelim = buildcc::env::get_os_envvar_delim();
  if constexpr (kDelim == nullptr) {
    buildcc::env::log_critical(__FUNCTION__, kOsNotSupported);
    return {};
  }

  std::vector<std::string> env_paths = buildcc::env::split(path_ptr, kDelim[0]);

  // DONE, Construct a directory iterator
  // Only take the files
  std::vector<fs::path> matches;
  for (const auto &env_p : env_paths) {
    std::error_code errcode;
    const auto dir_iter = fs::directory_iterator(env_p, errcode);
    if (errcode) {
      buildcc::env::log_critical(env_p, errcode.message());
      continue;
    }

    for (const auto &dir_entry : dir_iter) {
      if (!dir_entry.path().has_filename()) {
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
