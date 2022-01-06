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

#include "buildcc.h"

namespace buildcc {

struct ArgTargetInfo {
  std::string name;
  TargetType type;
  fs::path relative_to_root;
};

struct ArgTargetInputs {
  // Sources
  std::vector<fs::path> source_files;
  std::vector<fs::path> include_dirs;

  // External libs
  std::vector<fs::path> lib_dirs;
  std::vector<std::string> external_lib_deps;

  // Flags
  std::vector<std::string> preprocessor_flags;
  std::vector<std::string> common_compile_flags;
  std::vector<std::string> asm_compile_flags;
  std::vector<std::string> c_compile_flags;
  std::vector<std::string> cpp_compile_flags;
  std::vector<std::string> link_flags;
};

struct ArgScriptInfo {
  std::vector<std::string> configs;
};

void setup_arg_target_info(Args &args, ArgTargetInfo &out);
void setup_arg_target_inputs(Args &args, ArgTargetInputs &out);
void setup_arg_script_mode(Args &args, ArgScriptInfo &out);

} // namespace buildcc
