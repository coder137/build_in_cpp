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

#ifndef TARGET_API_INCLUDE_API_H_
#define TARGET_API_INCLUDE_API_H_

#include <filesystem>

#include "schema/path.h"

namespace fs = std::filesystem;

namespace buildcc::internal {

// Requires
// Toolchain
// User::Headers
// User::IncludeDirs
// TargetEnv
template <typename T> class IncludeApi {
public:
  std::vector<std::string> GetHeaderFiles() const {
    const auto &t = static_cast<const T &>(*this);
    return t.user_.headers.GetPaths();
  }

  const std::vector<std::string> &GetIncludeDirs() const {
    const auto &t = static_cast<const T &>(*this);
    return t.user_.include_dirs.GetPaths();
  }

  void AddHeaderAbsolute(const fs::path &absolute_filepath) {
    auto &t = static_cast<T &>(*this);

    t.toolchain_.GetConfig().ExpectsValidHeader(absolute_filepath);
    t.user_.headers.Emplace(absolute_filepath, "");
  }

  void GlobHeadersAbsolute(const fs::path &absolute_path) {
    auto &t = static_cast<T &>(*this);

    for (const auto &p : fs::directory_iterator(absolute_path)) {
      if (t.toolchain_.GetConfig().IsValidHeader(p.path())) {
        AddHeaderAbsolute(p.path());
      }
    }
  }

  void AddIncludeDirAbsolute(const fs::path &absolute_include_dir,
                             bool glob_headers = false) {
    auto &t = static_cast<T &>(*this);

    t.user_.include_dirs.Emplace(absolute_include_dir);

    if (glob_headers) {
      GlobHeadersAbsolute(absolute_include_dir);
    }
  }

  void AddHeader(const fs::path &relative_filename,
                 const fs::path &relative_to_target_path = "") {
    auto &t = static_cast<T &>(*this);

    // Check Source
    fs::path absolute_filepath =
        t.env_.GetTargetRootDir() / relative_to_target_path / relative_filename;
    AddHeaderAbsolute(absolute_filepath);
  }

  void GlobHeaders(const fs::path &relative_to_target_path = "") {
    auto &t = static_cast<T &>(*this);

    fs::path absolute_path =
        t.env_.GetTargetRootDir() / relative_to_target_path;
    GlobHeadersAbsolute(absolute_path);
  }

  void AddIncludeDir(const fs::path &relative_include_dir,
                     bool glob_headers = false) {
    auto &t = static_cast<T &>(*this);

    const fs::path absolute_include_dir =
        t.env_.GetTargetRootDir() / relative_include_dir;
    AddIncludeDirAbsolute(absolute_include_dir, glob_headers);
  }
};

} // namespace buildcc::internal

#endif
