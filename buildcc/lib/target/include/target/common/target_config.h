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

namespace fs = std::filesystem;

namespace buildcc {

struct TargetConfig {
  TargetConfig() = default;

  std::string target_ext{""};

  // clang-format off
  std::string pch_command{"{compiler} {preprocessor_flags} {include_dirs} {common_compile_flags} {pch_compile_flags} {compile_flags} -o {output} -c {input}"};
  std::string compile_command{"{compiler} {preprocessor_flags} {include_dirs} {common_compile_flags} {pch_object_flags} {compile_flags} -o {output} -c {input}"};
  std::string link_command{"{cpp_compiler} {link_flags} {compiled_sources} -o {output} {lib_dirs} {lib_deps}"};
  // clang-format on
};

} // namespace buildcc

#endif
