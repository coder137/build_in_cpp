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

#ifndef TARGET_API_PCH_API_H_
#define TARGET_API_PCH_API_H_

#include <filesystem>

#include "schema/path.h"

namespace fs = std::filesystem;

namespace buildcc::internal {

// Requires
// Toolchain
// User::Pchs
// TargetEnv
template <typename T> class PchApi {
public:
  std::vector<std::string> GetPchFiles() const {
    const auto &t = static_cast<const T &>(*this);
    return t.user_.pchs.GetPaths();
  }

  void AddPchAbsolute(const fs::path &absolute_filepath) {
    auto &t = static_cast<T &>(*this);

    t.toolchain_.GetConfig().ExpectsValidHeader(absolute_filepath);

    const fs::path absolute_pch = fs::path(absolute_filepath).make_preferred();
    t.user_.pchs.Emplace(absolute_pch, "");
  }

  void AddPch(const fs::path &relative_filename,
              const fs::path &relative_to_target_path = "") {
    auto &t = static_cast<T &>(*this);

    // Compute the absolute source path
    fs::path absolute_pch =
        t.env_.GetTargetRootDir() / relative_to_target_path / relative_filename;

    AddPchAbsolute(absolute_pch);
  }
};

} // namespace buildcc::internal

#endif
