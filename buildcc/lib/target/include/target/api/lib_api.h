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

#ifndef TARGET_API_LIB_API_H_
#define TARGET_API_LIB_API_H_

#include <filesystem>
#include <string>
#include <vector>

#include "schema/path.h"

namespace fs = std::filesystem;

namespace buildcc {

class Target;

}

namespace buildcc::internal {

// Requires
// User::LibDirs
// User::Libs
// User::ExternalLibs
// TargetEnv
// Target::GetTargetPath
template <typename T> class LibApi {
public:
  std::vector<std::string> GetLibDeps() const {
    const auto &t = static_cast<const T &>(*this);
    return t.user_.libs.GetPaths();
  }

  const std::vector<std::string> &GetExternalLibDeps() const {
    const auto &t = static_cast<const T &>(*this);
    return t.user_.external_libs;
  }

  const std::vector<std::string> &GetLibDirs() const {
    const auto &t = static_cast<const T &>(*this);
    return t.user_.lib_dirs.GetPaths();
  }

  void AddLibDirAbsolute(const fs::path &absolute_lib_dir) {
    auto &t = static_cast<T &>(*this);
    t.user_.lib_dirs.Emplace(absolute_lib_dir);
  }

  void AddLibDir(const fs::path &relative_lib_dir) {
    auto &t = static_cast<T &>(*this);
    fs::path final_lib_dir = t.env_.GetTargetRootDir() / relative_lib_dir;
    AddLibDirAbsolute(final_lib_dir);
  }

  void AddLibDep(const std::string &lib_dep) {
    auto &t = static_cast<T &>(*this);
    t.user_.external_libs.push_back(lib_dep);
  }

  // Target class has been forward declared
  // This is because this file is meant to be used by `TargetInfo` and `Target`
  void AddLibDep(const Target &lib_dep);
};

} // namespace buildcc::internal

#endif
