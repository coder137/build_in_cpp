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

#include "target/friend/file_extension.h"

#include "env/assert_fatal.h"
#include "target/target.h"
#include "target/target_storer.h"
#include "toolchain/toolchain.h"

namespace buildcc::base {

// TODO, Refactor this to static
void FileExt::SetSourceState(Type type) {
  switch (type) {
  case FileExt::Type::Asm:
    target_.state_.contains_asm = true;
    break;
  case FileExt::Type::C:
    target_.state_.contains_c = true;
    break;
  case FileExt::Type::Cpp:
    target_.state_.contains_cpp = true;
    break;
  default:
    break;
  }
}

// Getters
// TODO, Make this static
// TODO, Pass in the Config class
FileExt::Type FileExt::GetType(const fs::path &filepath) const {
  if (!filepath.has_extension()) {
    return FileExt::Type::Invalid;
  }

  FileExt::Type type = FileExt::Type::Invalid;
  const std::string ext = filepath.extension().string();

  if (target_.GetConfig().valid_c_ext.count(ext) == 1) {
    type = FileExt::Type::C;
  } else if (target_.GetConfig().valid_cpp_ext.count(ext) == 1) {
    type = FileExt::Type::Cpp;
  } else if (target_.GetConfig().valid_asm_ext.count(ext) == 1) {
    type = FileExt::Type::Asm;
  } else if (target_.GetConfig().valid_header_ext.count(ext) == 1) {
    type = FileExt::Type::Header;
  }

  return type;
}

std::optional<std::string> FileExt::GetCompileFlags(
    FileExt::Type type,
    const std::unordered_map<Type, std::string> &relational_data) {
  const auto iter = relational_data.find(type);
  if (iter == relational_data.end()) {
    return {};
  }
  return iter->second;
}

std::optional<std::string>
FileExt::GetCompiler(FileExt::Type type, const base::Toolchain &toolchain) {

  switch (type) {
  case FileExt::Type::Asm:
    return toolchain.GetAsmCompiler();
    break;
  case FileExt::Type::C:
    return toolchain.GetCCompiler();
    break;
  case FileExt::Type::Cpp:
    return toolchain.GetCppCompiler();
    break;
  default:
    break;
  }
  return {};
}

std::string FileExt::ToString(Type type) {
  std::string str;
  switch (type) {
  case Type::Asm:
    str = "Type::Asm";
    break;
  case Type::C:
    str = "Type::C";
    break;
  case Type::Cpp:
    str = "Type::Cpp";
    break;
  case Type::Header:
    str = "Type::Header";
    break;
  case Type::Invalid:
  default:
    str = "Type::Invalid";
    break;
  }
  return str;
}

bool FileExt::IsValidSource(Type type) {
  bool valid = false;
  switch (type) {
  case FileExt::Type::Asm:
  case FileExt::Type::C:
  case FileExt::Type::Cpp:
    valid = true;
    break;
  case FileExt::Type::Header:
  default:
    valid = false;
    break;
  }
  return valid;
}

bool FileExt::IsValidHeader(Type type) {
  bool valid = false;
  switch (type) {
  case FileExt::Type::Header:
    valid = true;
    break;
  case FileExt::Type::Asm:
  case FileExt::Type::C:
  case FileExt::Type::Cpp:
  default:
    valid = false;
    break;
  }
  return valid;
}

} // namespace buildcc::base
