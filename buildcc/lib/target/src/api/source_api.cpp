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

#include "target/api/source_api.h"

#include "target/target_info.h"

namespace buildcc::internal {

template <typename T>
void SourceApi<T>::AddSourceAbsolute(const fs::path &absolute_source) {
  T &t = static_cast<T &>(*this);

  t.lock_.ExpectsUnlock(__FUNCTION__);
  t.toolchain_.GetConfig().ExpectsValidSource(absolute_source);
  t.user_.sources.emplace(fs::path(absolute_source).make_preferred());
}

template <typename T>
void SourceApi<T>::GlobSourcesAbsolute(const fs::path &absolute_source_dir) {
  T &t = static_cast<T &>(*this);

  for (const auto &p : fs::directory_iterator(absolute_source_dir)) {
    if (t.toolchain_.GetConfig().IsValidSource(p.path())) {
      AddSourceAbsolute(p.path());
    }
  }
}

template <typename T>
void SourceApi<T>::AddSource(
    const fs::path &relative_source,
    const std::filesystem::path &relative_to_target_path) {
  T &t = static_cast<T &>(*this);

  // Compute the absolute source path
  fs::path absolute_source =
      t.env_.GetTargetRootDir() / relative_to_target_path / relative_source;
  AddSourceAbsolute(absolute_source);
}

template <typename T>
void SourceApi<T>::GlobSources(const fs::path &relative_to_target_path) {
  T &t = static_cast<T &>(*this);

  fs::path absolute_input_path =
      t.env_.GetTargetRootDir() / relative_to_target_path;
  for (const auto &p : fs::directory_iterator(absolute_input_path)) {
    if (t.toolchain_.GetConfig().IsValidSource(p.path())) {
      AddSourceAbsolute(p.path());
    }
  }
}

//
template class SourceApi<TargetInfo>;

} // namespace buildcc::internal
