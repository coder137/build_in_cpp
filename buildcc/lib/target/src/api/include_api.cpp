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

#include "target/api/include_api.h"

#include "target/target_info.h"

namespace buildcc::internal {

template <typename T>
void IncludeApi<T>::AddHeaderAbsolute(const fs::path &absolute_filepath) {
  T &t = static_cast<T &>(*this);

  t.state_.ExpectsUnlock();
  t.config_.ExpectsValidHeader(absolute_filepath);
  t.storer_.current_header_files.user.insert(absolute_filepath);
}

template <typename T>
void IncludeApi<T>::AddHeader(const fs::path &relative_filename,
                              const fs::path &relative_to_target_path) {
  T &t = static_cast<T &>(*this);

  // Check Source
  fs::path absolute_filepath =
      t.env_.GetTargetRootDir() / relative_to_target_path / relative_filename;
  AddHeaderAbsolute(absolute_filepath);
}

template <typename T>
void IncludeApi<T>::GlobHeaders(const fs::path &relative_to_target_path) {
  T &t = static_cast<T &>(*this);

  fs::path absolute_path = t.env_.GetTargetRootDir() / relative_to_target_path;
  GlobHeadersAbsolute(absolute_path);
}

template <typename T>
void IncludeApi<T>::GlobHeadersAbsolute(const fs::path &absolute_path) {
  T &t = static_cast<T &>(*this);

  for (const auto &p : fs::directory_iterator(absolute_path)) {
    if (t.config_.IsValidHeader(p.path())) {
      AddHeaderAbsolute(p.path());
    }
  }
}

template <typename T>
void IncludeApi<T>::AddIncludeDir(const fs::path &relative_include_dir,
                                  bool glob_headers) {
  T &t = static_cast<T &>(*this);

  const fs::path absolute_include_dir =
      t.env_.GetTargetRootDir() / relative_include_dir;
  AddIncludeDirAbsolute(absolute_include_dir, glob_headers);
}

template <typename T>
void IncludeApi<T>::AddIncludeDirAbsolute(const fs::path &absolute_include_dir,
                                          bool glob_headers) {
  T &t = static_cast<T &>(*this);

  t.state_.ExpectsUnlock();
  t.storer_.current_include_dirs.insert(absolute_include_dir);

  if (glob_headers) {
    GlobHeadersAbsolute(absolute_include_dir);
  }
}

template class IncludeApi<TargetInfo>;

} // namespace buildcc::internal
