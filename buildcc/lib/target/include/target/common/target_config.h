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

#ifndef TARGET_COMMON_TARGET_CONFIG_H_
#define TARGET_COMMON_TARGET_CONFIG_H_

#include <filesystem>
#include <string>
#include <unordered_set>

#include "target/common/target_file_ext.h"

namespace fs = std::filesystem;

namespace buildcc {

struct TargetConfig {
  TargetConfig() = default;

  /**
   * @brief Get the valid file extension from a path
   *
   * See TargetConfig::valid_c_ext, TargetConfig::valid_cpp_ext,
   * TargetConfig::valid_asm_ext, TargetConfig::valid_header_ext
   *
   * @param filepath Absolute / Relative path of the file
   * @return TargetFileExt File path detected as per TargetConfig::valid_*
   * variables
   */
  TargetFileExt GetFileExt(const fs::path &filepath) const;

  /**
   * @brief Checks for C/C++ source file validity.
   *
   * See TargetConfig::valid_c_ext, TargetConfig::valid_cpp_ext,
   * TargetConfig::valid_asm_ext
   *
   * @param filepath Absolute / Relative path of file
   * @return true If file extension belongs to the above valid_* list
   * @return false If file extension does not belong to the above valid_* list
   */
  bool IsValidSource(const fs::path &filepath) const;

  /**
   * @brief Checks for Header file validity
   *
   * See TargetConfig::valid_header_ext
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

  std::string target_ext{""};
  std::string obj_ext{".o"};
  std::string pch_header_ext{".h"};
  std::string pch_compile_ext{".gch"};

  std::string prefix_include_dir{"-I"};
  std::string prefix_lib_dir{"-L"};

  // clang-format off
  std::string pch_command{"{compiler} {preprocessor_flags} {include_dirs} {common_compile_flags} {pch_compile_flags} {compile_flags} -o {output} -c {input}"};
  std::string compile_command{"{compiler} {preprocessor_flags} {include_dirs} {common_compile_flags} {pch_object_flags} {compile_flags} -o {output} -c {input}"};
  std::string link_command{"{cpp_compiler} {link_flags} {compiled_sources} -o {output} {lib_dirs} {lib_deps}"};
  // clang-format on

  std::unordered_set<std::string> valid_c_ext{".c"};
  std::unordered_set<std::string> valid_cpp_ext{".cpp", ".cxx", ".cc"};
  std::unordered_set<std::string> valid_asm_ext{".s", ".S", ".asm"};
  std::unordered_set<std::string> valid_header_ext{".h", ".hpp"};
};

} // namespace buildcc

#endif
