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

#include "env/assert_fatal.h"

#include "target/common/path.h"

#include "fmt/format.h"

namespace buildcc::base {

TargetFileExt TargetConfig::GetFileExt(const fs::path &filepath) const {
  if (!filepath.has_extension()) {
    return TargetFileExt::Invalid;
  }

  TargetFileExt type = TargetFileExt::Invalid;
  const std::string ext = filepath.extension().string();

  if (valid_c_ext.count(ext) == 1) {
    type = TargetFileExt::C;
  } else if (valid_cpp_ext.count(ext) == 1) {
    type = TargetFileExt::Cpp;
  } else if (valid_asm_ext.count(ext) == 1) {
    type = TargetFileExt::Asm;
  } else if (valid_header_ext.count(ext) == 1) {
    type = TargetFileExt::Header;
  }

  return type;
}

bool TargetConfig::IsValidSource(const fs::path &filepath) const {
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

void TargetConfig::ExpectsValidSource(const fs::path &filepath) const {
  env::assert_fatal(
      IsValidSource(filepath),
      fmt::format("{} does not have a valid source extension", filepath));
}

bool TargetConfig::IsValidHeader(const fs::path &filepath) const {
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

void TargetConfig::ExpectsValidHeader(const fs::path &filepath) const {
  env::assert_fatal(
      IsValidHeader(filepath),
      fmt::format("{} does not have a valid header extension", filepath));
}

} // namespace buildcc::base
