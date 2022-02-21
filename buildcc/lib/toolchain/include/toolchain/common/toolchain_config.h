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

#ifndef TOOLCHAIN_COMMON_TOOLCHAIN_CONFIG_H_
#define TOOLCHAIN_COMMON_TOOLCHAIN_CONFIG_H_

#include <filesystem>
#include <string>
#include <unordered_set>

#include "toolchain/common/file_ext.h"

namespace fs = std::filesystem;

namespace buildcc {

struct ToolchainConfig {
  ToolchainConfig() = default;

  /**
   * @brief Get the valid file extension from a path
   *
   * See ToolchainConfig::valid_c_ext, ToolchainConfig::valid_cpp_ext,
   * ToolchainConfig::valid_asm_ext, ToolchainConfig::valid_header_ext
   *
   * @param filepath Absolute / Relative path of the file
   * @return FileExt File path detected as per Toolchain::valid_*
   * variables
   */
  FileExt GetFileExt(const fs::path &filepath) const;

  /**
   * @brief Checks for C/C++ source file validity.
   *
   * See ToolchainConfig::valid_c_ext, ToolchainConfig::valid_cpp_ext,
   * ToolchainConfig::valid_asm_ext
   *
   * @param filepath Absolute / Relative path of file
   * @return true If file extension belongs to the above valid_* list
   * @return false If file extension does not belong to the above valid_* list
   */
  bool IsValidSource(const fs::path &filepath) const;

  /**
   * @brief Checks for Header file validity
   *
   * See ToolchainConfig::valid_header_ext
   *
   * @param filepath Absolute / Relative path of file
   * @return true If file extension belongs to above valid_* list
   * @return false If file extension does not belong to above valid_* list
   */
  bool IsValidHeader(const fs::path &filepath) const;

  /**
   * @brief Expects Source file validity
   *
   * env::assert_fatal if not a valid source
   *
   * @param filepath Absolute / Relative path of file
   */
  void ExpectsValidSource(const fs::path &filepath) const;

  /**
   * @brief Expects header file validity
   *
   * env::assert_fatal if not a valid header
   *
   * @param filepath Absolute / Relative path of file
   */
  void ExpectsValidHeader(const fs::path &filepath) const;

  std::string obj_ext{".o"};
  std::string pch_header_ext{".h"};
  std::string pch_compile_ext{".gch"};

  std::string prefix_include_dir{"-I"};
  std::string prefix_lib_dir{"-L"};

  std::unordered_set<std::string> valid_c_ext{".c"};
  std::unordered_set<std::string> valid_cpp_ext{".cpp", ".cxx", ".cc"};
  std::unordered_set<std::string> valid_asm_ext{".s", ".S", ".asm"};
  std::unordered_set<std::string> valid_header_ext{".h", ".hpp"};
};

} // namespace buildcc

#endif
