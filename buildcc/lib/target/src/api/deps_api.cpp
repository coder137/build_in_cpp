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

#include "target/api/deps_api.h"

#include "target/target_info.h"

namespace buildcc::internal {

template <typename T>
void DepsApi<T>::AddCompileDependencyAbsolute(const fs::path &absolute_path) {
  T &t = static_cast<T &>(*this);

  t.state_.ExpectsUnlock();
  t.storer_.current_compile_dependencies.user.insert(absolute_path);
}
template <typename T>
void DepsApi<T>::AddLinkDependencyAbsolute(const fs::path &absolute_path) {
  T &t = static_cast<T &>(*this);

  t.state_.ExpectsUnlock();
  t.storer_.current_link_dependencies.user.insert(absolute_path);
}

template <typename T>
void DepsApi<T>::AddCompileDependency(const fs::path &relative_path) {
  T &t = static_cast<T &>(*this);

  fs::path absolute_path = t.env_.GetTargetRootDir() / relative_path;
  AddCompileDependencyAbsolute(absolute_path);
}

template <typename T>
void DepsApi<T>::AddLinkDependency(const fs::path &relative_path) {
  T &t = static_cast<T &>(*this);

  fs::path absolute_path = t.env_.GetTargetRootDir() / relative_path;
  AddLinkDependencyAbsolute(absolute_path);
}

template class DepsApi<TargetInfo>;

} // namespace buildcc::internal
