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

#include "target/api/lib_api.h"

#include "target/target.h"

namespace buildcc::base {

template <typename T>
void LibApi<T>::AddLibDir(const fs::path &relative_lib_dir) {
  T &t = static_cast<T &>(*this);

  fs::path final_lib_dir = t.env_.GetTargetRootDir() / relative_lib_dir;
  AddLibDirAbsolute(final_lib_dir);
}

template <typename T>
void LibApi<T>::AddLibDirAbsolute(const fs::path &absolute_lib_dir) {
  T &t = static_cast<T &>(*this);

  t.state_.ExpectsUnlock();
  t.storer_.current_lib_dirs.insert(absolute_lib_dir);
}

template <typename T> void LibApi<T>::AddLibDep(const Target &lib_dep) {
  T &t = static_cast<T &>(*this);

  t.state_.ExpectsUnlock();
  t.storer_.current_lib_deps.user.insert(lib_dep.GetTargetPath());
}

template <typename T> void LibApi<T>::AddLibDep(const std::string &lib_dep) {
  T &t = static_cast<T &>(*this);

  t.state_.ExpectsUnlock();
  t.storer_.current_external_lib_deps.insert(lib_dep);
}

template class LibApi<Target>;

} // namespace buildcc::base
