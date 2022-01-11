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

#ifndef ENV_HOST_COMPILER_H_
#define ENV_HOST_COMPILER_H_

// https://sourceforge.net/p/predef/wiki/OperatingSystems/
// https://abseil.io/docs/cpp/platforms/macros
namespace buildcc::env {

inline constexpr bool is_gcc() {
#if defined(__GNUC__)
  return true;
#else
  return false;
#endif
}

inline constexpr bool is_mingw() {
#if defined(__MINGW32__) || defined(__MINGW64__)
  return true;
#else
  return false;
#endif
}

inline constexpr bool is_clang() {
#if defined(__clang__)
  return true;
#else
  return false;
#endif
}

inline constexpr bool is_msvc() {
#if defined(_MSC_VER)
  return true;
#else
  return false;
#endif
}

} // namespace buildcc::env

#endif
