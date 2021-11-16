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

#include "target/common/target_config.h"

namespace buildcc::base {

bool TargetConfig::IsValidSource(const fs::path &filepath) {
  if (!filepath.has_extension()) {
    return false;
  }

  const std::string ext = filepath.extension().string();
  bool valid = false;
  if ((valid_c_ext.find(ext) != valid_c_ext.end()) ||
      (valid_cpp_ext.find(ext) != valid_cpp_ext.end()) ||
      (valid_asm_ext.find(ext) != valid_asm_ext.end())) {
    valid = true;
  }
  return valid;
}

bool TargetConfig::IsValidHeader(const fs::path &filepath) {
  if (!filepath.has_extension()) {
    return {};
  }

  const std::string ext = filepath.extension().string();
  bool valid = false;
  if ((valid_header_ext.find(ext) != valid_header_ext.end())) {
    valid = true;
  }
  return valid;
}

} // namespace buildcc::base
