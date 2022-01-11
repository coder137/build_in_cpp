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

bool IsValidTargetType(buildcc::base::TargetType type) {
  switch (type) {
  case buildcc::base::TargetType::Executable:
  case buildcc::base::TargetType::StaticLibrary:
  case buildcc::base::TargetType::DynamicLibrary:
    return true;
    break;
  default:
    return false;
    break;
  }
}

} // namespace

namespace buildcc::base {

void Target::Initialize() {
  // Checks
  env::assert_fatal(
      env::is_init(),
      "Environment is not initialized. Use the buildcc::env::init API");
  env::assert_fatal(IsValidTargetType(type_), "Invalid Target Type");
  fs::create_directories(GetTargetBuildDir());

  // String updates
  unique_id_ = fmt::format("[{}] {}", toolchain_.GetName(), name_);
  std::string path = fmt::format(
      "{}", GetTargetPath().lexically_relative(env::get_project_build_dir()));
  tf_.name(path);
}

// Rechecks
void Target::RecheckPaths(const internal::path_unordered_set &previous_path,
                          const internal::path_unordered_set &current_path) {
  BuilderInterface::RecheckPaths(
      previous_path, current_path, [&]() { PathRemoved(); },
      [&]() { PathAdded(); }, [&]() { PathUpdated(); });
}

void Target::RecheckDirs(const fs_unordered_set &previous_dirs,
                         const fs_unordered_set &current_dirs) {
  RecheckChanged(previous_dirs, current_dirs,
                 std::bind(&Target::DirChanged, this));
}

void Target::RecheckFlags(
    const std::unordered_set<std::string> &previous_flags,
    const std::unordered_set<std::string> &current_flags) {
  RecheckChanged(previous_flags, current_flags,
                 std::bind(&Target::FlagChanged, this));
}

void Target::RecheckExternalLib(
    const std::unordered_set<std::string> &previous_external_libs,
    const std::unordered_set<std::string> &current_external_libs) {
  RecheckChanged(previous_external_libs, current_external_libs,
                 std::bind(&Target::ExternalLibChanged, this));
}

std::optional<std::string> Target::SelectCompileFlags(TargetFileExt ext) const {
  switch (ext) {
  case TargetFileExt::Asm:
    return internal::aggregate(GetAsmCompileFlags());
    break;
  case TargetFileExt::C:
    return internal::aggregate(GetCCompileFlags());
    break;
  case TargetFileExt::Cpp:
    return internal::aggregate(GetCppCompileFlags());
    break;
  default:
    break;
  }
  return {};
}

std::optional<std::string> Target::SelectCompiler(TargetFileExt ext) const {
  switch (ext) {
  case TargetFileExt::Asm:
    return GetToolchain().GetAsmCompiler();
    break;
  case TargetFileExt::C:
    return GetToolchain().GetCCompiler();
    break;
  case TargetFileExt::Cpp:
    return GetToolchain().GetCppCompiler();
    break;
  default:
    break;
  }
  return {};
}

} // namespace buildcc::base
