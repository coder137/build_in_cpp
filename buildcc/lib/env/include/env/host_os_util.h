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

#ifndef ENV_HOST_OS_UTIL_H_
#define ENV_HOST_OS_UTIL_H_

#include <stdlib.h>

#include "host_os.h"
#include "logging.h"

// https://askubuntu.com/questions/156392/what-is-the-equivalent-of-an-exe-file
namespace buildcc::env {

// Common
constexpr const char *const kRaiseIssueStr =
    "Unknown operating system, returning nullptr. Raise an "
    "issue at http://github.com/coder137/build_in_cpp";

// OS Path delimiters
constexpr const char *const kWinEnvDelim = ";";
constexpr const char *const kUnixEnvDelim = ":";

/**
 * @brief Get the OS environment variable delimiter
 * ; for windows
 * : for linux, unix and mac
 *
 * @return constexpr char const* for supported operating systems
 * @return nullptr otherwise with a critical message to raise an issue
 */
inline constexpr char const *get_os_envvar_delim() {
  if constexpr (is_win()) {
    return kWinEnvDelim;
  } else if constexpr (is_linux() || is_unix() || is_mac()) {
    return kUnixEnvDelim;
  }
  log_critical(__FUNCTION__, kRaiseIssueStr);
  return nullptr;
}

// OS executable extensions
constexpr const char *const kWinExecutableExt = ".exe";
constexpr const char *const kUnixExecutableExt = "";

/**
 * @brief Get the OS executable extension
 * ".exe" for windows
 * "" for linux, unix and mac
 *
 * @return constexpr const char*  for supported operating systems
 * @return nullptr otherwise with a critical message to raise an issue
 */
inline constexpr const char *get_os_executable_extension() {
  if constexpr (is_win()) {
    return kWinExecutableExt;
  } else if constexpr (is_linux() || is_unix() || is_mac()) {
    return kUnixExecutableExt;
  }

  log_critical(__FUNCTION__, kRaiseIssueStr);
  return nullptr;
}

} // namespace buildcc::env

#endif
