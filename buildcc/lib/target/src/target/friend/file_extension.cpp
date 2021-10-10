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

#include "target/target.h"

namespace buildcc::base {

void FileExt::SetSourceState(Type type) {
  switch (type) {
  case FileExt::Type::Asm:
    target_.state_.contains_asm_src = true;
    break;
  case FileExt::Type::C:
    target_.state_.contains_c_src = true;
    break;
  case FileExt::Type::Cpp:
    target_.state_.contains_cpp_src = true;
    break;
  default:
    break;
  }
}

// Getters
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

std::optional<std::string> FileExt::GetCompileFlags(FileExt::Type type) const {
  switch (type) {
  case FileExt::Type::Asm:
    return internal::aggregate(target_.GetCurrentAsmCompileFlags());
    break;
  case FileExt::Type::C:
    return internal::aggregate(target_.GetCurrentCCompileFlags());
    break;
  case FileExt::Type::Cpp:
    return internal::aggregate(target_.GetCurrentCppCompileFlags());
    break;
  default:
    break;
  }
  return {};
}

std::optional<std::string> FileExt::GetCompiler(FileExt::Type type) const {
  switch (type) {
  case FileExt::Type::Asm:
    return target_.GetToolchain().GetAsmCompiler();
    break;
  case FileExt::Type::C:
    return target_.GetToolchain().GetCCompiler();
    break;
  case FileExt::Type::Cpp:
    return target_.GetToolchain().GetCppCompiler();
    break;
  default:
    break;
  }
  return {};
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
