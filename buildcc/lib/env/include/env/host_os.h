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

#ifndef ENV_HOST_OS_H_
#define ENV_HOST_OS_H_

// https://sourceforge.net/p/predef/wiki/OperatingSystems/
// https://web.archive.org/web/20191012035921/http://nadeausoftware.com/articles/2012/01/c_c_tip_how_use_compiler_predefined_macros_detect_operating_system
namespace buildcc::env {

// TODO, Add as required
// enum class OsType {
//   UNIX,
//   LINUX,
//   WIN,
//   MACOS,
// };

// TODO, Add as required
// enum class StandardType {
//   POSIX,
//   UNIX,
// };

inline constexpr bool is_linux() {
#if defined(__linux__)
  return true;
#else
  return false;
#endif
}

inline constexpr bool is_win() {
#if defined(_WIN32)
  return true;
#else
  return false;
#endif
}

inline constexpr bool is_mac() {
#if defined(__APPLE__) || defined(__MACH__)
  return true;
#else
  return false;
#endif
}

inline constexpr bool is_unix() {
#if defined(__unix__)
  return true;
#else
  return false;
#endif
}

} // namespace buildcc::env

#endif
