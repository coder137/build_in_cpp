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

#ifndef TARGET_TARGET_STORER_H_
#define TARGET_TARGET_STORER_H_

#include <unordered_set>

#include "target/path.h"

namespace buildcc::internal {

struct TargetStorer {
  internal::default_files current_source_files;
  internal::default_files current_header_files;
  internal::default_files current_pch_files;
  internal::default_files current_lib_deps;

  internal::fs_unordered_set current_include_dirs;
  internal::fs_unordered_set current_lib_dirs;

  std::unordered_set<std::string> current_external_lib_deps;

  std::unordered_set<std::string> current_preprocessor_flags;
  std::unordered_set<std::string> current_common_compile_flags;
  std::unordered_set<std::string> current_pch_compile_flags;
  std::unordered_set<std::string> current_pch_object_flags;
  std::unordered_set<std::string> current_asm_compile_flags;
  std::unordered_set<std::string> current_c_compile_flags;
  std::unordered_set<std::string> current_cpp_compile_flags;
  std::unordered_set<std::string> current_link_flags;

  internal::default_files current_compile_dependencies;
  internal::default_files current_link_dependencies;
};

} // namespace buildcc::internal

#endif
