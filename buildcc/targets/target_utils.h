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

#ifndef TARGETS_TARGET_UTILS_H_
#define TARGETS_TARGET_UTILS_H_

#include <string>

#include "target/target.h"

#include "env/host_os.h"

namespace buildcc {

// DONE, GCC
// DONE, MSVC
// TODO, CLANG
inline std::string_view Extension(base::TargetType type,
                                  base::Toolchain::Id id) {
  switch (id) {
  // Toolchain ID: GCC
  // Target Type: Executable, StaticLib, DynamicLib
  // OS: Linux == Macos, Windows
  case base::Toolchain::Id::Gcc: {
    switch (type) {
    case base::TargetType::StaticLibrary:
      return kGccStaticLibExt;
    case base::TargetType::DynamicLibrary:
      return kGccDynamicLibExt;
    case base::TargetType::Executable:
      if constexpr (env::is_win()) {
        return kWinExecutableExt;
      }
    default:
      break;
    }
  } break;
  // Toolchain ID: MSVC
  // Target Type: Executable, StaticLib, DynamicLib
  // OS: Windows
  case base::Toolchain::Id::Msvc:
    switch (type) {
    case base::TargetType::StaticLibrary:
      return kWinStaticLibExt;
    case base::TargetType::DynamicLibrary:
      return kWinDynamicLibExt;
    case base::TargetType::Executable:
      return kWinExecutableExt;
    default:
      break;
    }
    break;
  default:
    break;
  }
  return "";
}

inline std::string Name(const std::string &name, base::TargetType type,
                        const base::Toolchain &toolchain) {
  return fmt::format("{}{}", name, Extension(type, toolchain.GetId()));
}

} // namespace buildcc

#endif
