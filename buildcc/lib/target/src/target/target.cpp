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

#include "target/target.h"

// Internal
#include "target/common/util.h"

// Env
#include "env/assert_fatal.h"

// Fmt
#include "fmt/format.h"

namespace fs = std::filesystem;

namespace {

bool IsValidTargetType(buildcc::TargetType type) {
  switch (type) {
  case buildcc::TargetType::Executable:
  case buildcc::TargetType::StaticLibrary:
  case buildcc::TargetType::DynamicLibrary:
    return true;
    break;
  default:
    return false;
    break;
  }
}

} // namespace

namespace buildcc {

void Target::Initialize() {
  // Checks
  env::assert_fatal(
      Project::IsInit(),
      "Environment is not initialized. Use the buildcc::Project::Init API");
  env::assert_fatal(IsValidTargetType(type_), "Invalid Target Type");
  fs::create_directories(GetTargetBuildDir());

  // String updates
  unique_id_ = fmt::format("[{}] {}", toolchain_.GetName(), name_);
  std::string path = fmt::format(
      "{}", GetTargetPath().lexically_relative(Project::GetBuildDir()));
  tf_.name(path);
}

env::optional<std::string> Target::SelectCompileFlags(FileExt ext) const {
  switch (ext) {
  case FileExt::Asm:
    return internal::aggregate(GetAsmCompileFlags());
    break;
  case FileExt::C:
    return internal::aggregate(GetCCompileFlags());
    break;
  case FileExt::Cpp:
    return internal::aggregate(GetCppCompileFlags());
    break;
  default:
    break;
  }
  return {};
}

env::optional<std::string> Target::SelectCompiler(FileExt ext) const {
  switch (ext) {
  case FileExt::Asm:
    return GetToolchain().GetAssembler();
    break;
  case FileExt::C:
    return GetToolchain().GetCCompiler();
    break;
  case FileExt::Cpp:
    return GetToolchain().GetCppCompiler();
    break;
  default:
    break;
  }
  return {};
}

} // namespace buildcc
