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

#ifndef TOOLCHAIN_COMMON_TOOLCHAIN_ID_H_
#define TOOLCHAIN_COMMON_TOOLCHAIN_ID_H_

#include "fmt/format.h"

namespace buildcc {

enum class ToolchainId {
  Gcc = 0,   ///< GCC Toolchain
  Msvc,      ///< MSVC Toolchain
  Clang,     ///< Clang Toolchain
  MinGW,     ///< MinGW Toolchain (Similar to GCC, but for Windows)
  Custom,    ///< Custom Toolchain not defined in this list
  Undefined, ///< Default value when unknown
};

} // namespace buildcc

template <>
struct fmt::formatter<buildcc::ToolchainId> : formatter<std::string> {
  template <typename FormatContext>
  auto format(buildcc::ToolchainId id, FormatContext &ctx) {
    std::string id_name;
    switch (id) {
    case buildcc::ToolchainId::Gcc:
      id_name = "Gcc";
      break;
    case buildcc::ToolchainId::Msvc:
      id_name = "Msvc";
      break;
    case buildcc::ToolchainId::Clang:
      id_name = "Clang";
      break;
    case buildcc::ToolchainId::MinGW:
      id_name = "MinGW";
      break;
    case buildcc::ToolchainId::Custom:
      id_name = "Custom";
      break;
    case buildcc::ToolchainId::Undefined:
    default:
      id_name = "Undefined";
      break;
    }
    return formatter<std::string>::format(id_name, ctx);
  }
};

#endif
