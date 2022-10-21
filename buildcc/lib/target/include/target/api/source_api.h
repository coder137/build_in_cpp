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

#ifndef TARGET_API_SOURCE_API_H_
#define TARGET_API_SOURCE_API_H_

#include <filesystem>

#include "schema/path.h"

namespace fs = std::filesystem;

namespace buildcc::internal {

// Requires
// Toolchain
// User::Sources
// TargetEnv
template <typename T> class SourceApi {
public:
  const fs_unordered_set &GetSourceFiles() const {
    const auto &t = static_cast<const T &>(*this);
    return t.user_.sources;
  }

  void AddSourceAbsolute(const fs::path &absolute_source) {
    auto &t = static_cast<T &>(*this);

    t.toolchain_.GetConfig().ExpectsValidSource(absolute_source);
    t.user_.sources.emplace(fs::path(absolute_source).make_preferred());
  }

  void GlobSourcesAbsolute(const fs::path &absolute_source_dir) {
    auto &t = static_cast<T &>(*this);

    for (const auto &p : fs::directory_iterator(absolute_source_dir)) {
      if (t.toolchain_.GetConfig().IsValidSource(p.path())) {
        AddSourceAbsolute(p.path());
      }
    }
  }

  void AddSource(const fs::path &relative_source,
                 const fs::path &relative_to_target_path = "") {
    auto &t = static_cast<T &>(*this);

    // Compute the absolute source path
    fs::path absolute_source =
        t.env_.GetTargetRootDir() / relative_to_target_path / relative_source;
    AddSourceAbsolute(absolute_source);
  }

  void GlobSources(const fs::path &relative_to_target_path = "") {
    auto &t = static_cast<T &>(*this);

    fs::path absolute_input_path =
        t.env_.GetTargetRootDir() / relative_to_target_path;
    for (const auto &p : fs::directory_iterator(absolute_input_path)) {
      if (t.toolchain_.GetConfig().IsValidSource(p.path())) {
        AddSourceAbsolute(p.path());
      }
    }
  }
};

} // namespace buildcc::internal

#endif
