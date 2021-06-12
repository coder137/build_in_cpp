/*
 * Copyright 2021 Niket Naidu All rights reserved.
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

#include "target.h"

// Internal
#include "internal/util.h"

// Env
#include "assert_fatal.h"

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

// PROTECTED

// Getters

FileExtType Target::GetFileExtType(const fs::path &filepath) const {
  if (!filepath.has_extension()) {
    return FileExtType::Invalid;
  }

  FileExtType type = FileExtType::Invalid;
  const std::string ext = filepath.extension().string();

  if (valid_c_ext_.count(ext) == 1) {
    type = FileExtType::C;
  } else if (valid_cpp_ext_.count(ext) == 1) {
    type = FileExtType::Cpp;
  } else if (valid_asm_ext_.count(ext) == 1) {
    type = FileExtType::Asm;
  } else if (valid_header_ext_.count(ext) == 1) {
    type = FileExtType::Header;
  }

  return type;
}

bool Target::IsValidSource(const fs::path &sourcepath) const {
  bool valid = false;
  switch (GetFileExtType(sourcepath)) {
  case FileExtType::Asm:
  case FileExtType::C:
  case FileExtType::Cpp:
    valid = true;
    break;
  case FileExtType::Header:
  default:
    valid = false;
    break;
  }
  return valid;
}

bool Target::IsValidHeader(const fs::path &headerpath) const {
  bool valid = false;
  switch (GetFileExtType(headerpath)) {
  case FileExtType::Header:
    valid = true;
    break;
  case FileExtType::Asm:
  case FileExtType::C:
  case FileExtType::Cpp:
  default:
    valid = false;
    break;
  }
  return valid;
}

// TODO, Since we are sanitizing the input source files when adding we might
// only need to check for valid sources (ASM, C, CPP)
// i.e we can eliminate the default case
const std::string &Target::GetCompiler(const fs::path &source) const {
  switch (GetFileExtType(source)) {
  case FileExtType::Asm:
    return toolchain_.GetAsmCompiler();
    break;
  case FileExtType::C:
    return toolchain_.GetCCompiler();
    break;
  case FileExtType::Cpp:
    break;
  default:
    buildcc::env::assert_fatal(
        false, fmt::format("Invalid source {}", source.string()));
    break;
  }
  return toolchain_.GetCppCompiler();
}

const internal::Path &
Target::GetCompiledSourcePath(const fs::path &source) const {
  const auto fiter = current_object_files_.find(source.native());
  env::assert_fatal(fiter != current_object_files_.end(),
                    fmt::format("{} not found", source.string()));
  return current_object_files_.at(source.native());
}

internal::path_unordered_set Target::GetCompiledSources() const {
  internal::path_unordered_set compiled_sources;
  for (const auto &p : current_object_files_) {
    compiled_sources.insert(p.second);
  }
  return compiled_sources;
}

// PRIVATE

void Target::Initialize() {
  // Checks
  env::assert_fatal(
      env::is_init(),
      "Environment is not initialized. Use the buildcc::env::init API");
  env::assert_fatal(IsValidTargetType(type_), "Invalid Target Type");
  fs::create_directories(target_intermediate_dir_);

  // Taskflow parameters
  tf_.name(name_);
}

// Rechecks
void Target::RecheckPaths(const internal::path_unordered_set &previous_path,
                          const internal::path_unordered_set &current_path) {
  // * Compile sources / Target already requires rebuild
  if (dirty_) {
    return;
  }

  // * Old path is removed
  const bool removed =
      internal::is_previous_paths_different(previous_path, current_path);
  if (removed) {
    PathRemoved();
    dirty_ = true;
    return;
  }

  for (const auto &path : current_path) {
    auto iter = previous_path.find(path);

    if (iter == previous_path.end()) {
      // * New path added
      PathAdded();
      dirty_ = true;
    } else {
      // * Path is updated
      if (path.GetLastWriteTimestamp() > iter->GetLastWriteTimestamp()) {
        PathUpdated();
        dirty_ = true;
      } else {
        // * Do nothing
      }
    }

    if (dirty_) {
      break;
    }
  }
}

void Target::RecheckDirs(const std::unordered_set<std::string> &previous_dirs,
                         const std::unordered_set<std::string> &current_dirs) {
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

void Target::RecheckChanged(const std::unordered_set<std::string> &previous,
                            const std::unordered_set<std::string> &current,
                            std::function<void(void)> callback) {
  if (dirty_) {
    return;
  }

  if (previous != current) {
    callback();
    dirty_ = true;
  }
}

} // namespace buildcc::base
