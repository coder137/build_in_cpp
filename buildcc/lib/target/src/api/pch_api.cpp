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

#include "target/api/pch_api.h"

#include "target/target_info.h"

namespace buildcc::internal {

template <typename T>
void PchApi<T>::AddPchAbsolute(const fs::path &absolute_filepath) {
  T &t = static_cast<T &>(*this);

  t.toolchain_.GetConfig().ExpectsValidHeader(absolute_filepath);

  const fs::path absolute_pch = fs::path(absolute_filepath).make_preferred();
  t.user_.pchs.insert(absolute_pch);
}

template <typename T>
void PchApi<T>::AddPch(const fs::path &relative_filename,
                       const fs::path &relative_to_target_path) {
  T &t = static_cast<T &>(*this);

  // Compute the absolute source path
  fs::path absolute_pch =
      t.env_.GetTargetRootDir() / relative_to_target_path / relative_filename;

  AddPchAbsolute(absolute_pch);
}

template class PchApi<TargetInfo>;

} // namespace buildcc::internal
