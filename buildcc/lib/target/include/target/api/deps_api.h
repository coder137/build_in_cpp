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

#ifndef TARGET_API_DEPS_API_H_
#define TARGET_API_DEPS_API_H_

#include <filesystem>

#include "schema/path.h"

namespace fs = std::filesystem;

namespace buildcc::internal {

// Requires
// User::CompileDependencies
// User::LinkDependencies
// TargetEnv
template <typename T> class DepsApi {
public:
  // TODO, AddPchDependency
  // TODO, Rename AddObjectDependency
  // TODO, Rename AddTargetDependency

  const fs_unordered_set &GetCompileDependencies() const {
    const auto &t = static_cast<const T &>(*this);
    return t.user_.compile_dependencies;
  }

  const fs_unordered_set &GetLinkDependencies() const {
    const auto &t = static_cast<const T &>(*this);
    return t.user_.link_dependencies;
  }

  /**
   * @brief Recompile sources to object if compile dependency is removed, added
   * or newer from the previous build
   */
  void AddCompileDependencyAbsolute(const fs::path &absolute_path) {
    auto &t = static_cast<T &>(*this);

    t.user_.compile_dependencies.insert(absolute_path);
  }

  /**
   * @brief Recompile sources to object if compile dependency is removed, added
   * or newer from the previous build
   */
  void AddCompileDependency(const fs::path &relative_path) {
    auto &t = static_cast<T &>(*this);

    fs::path absolute_path = t.env_.GetTargetRootDir() / relative_path;
    AddCompileDependencyAbsolute(absolute_path);
  }

  /**
   * @brief Relink target if link dependency is removed, added or newer from
   * previous build
   */
  void AddLinkDependencyAbsolute(const fs::path &absolute_path) {
    auto &t = static_cast<T &>(*this);

    t.user_.link_dependencies.insert(absolute_path);
  }

  /**
   * @brief Relink target if link dependency is removed, added or newer from
   * previous build
   */
  void AddLinkDependency(const fs::path &relative_path) {
    auto &t = static_cast<T &>(*this);

    fs::path absolute_path = t.env_.GetTargetRootDir() / relative_path;
    AddLinkDependencyAbsolute(absolute_path);
  }
};

} // namespace buildcc::internal

#endif
