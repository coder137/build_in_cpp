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

#ifndef TARGET_API_RECHECK_API_H_
#define TARGET_API_RECHECK_API_H_

#include "target/common/path.h"

namespace buildcc::internal {

template <typename T>
inline bool recheck_changed(const T &previous, const T &current) {
  bool changed = false;
  if (previous != current) {
    changed = true;
  }
  return changed;
}

enum class PathChanged {
  kPathNotChanged,
  kPathRemoved,
  kPathAdded,
  kPathUpdated
};

inline PathChanged
recheck_path_changed(const path_unordered_set &previous_path,
                     const path_unordered_set &current_path) {
  // * Old path is removed
  PathChanged path_changed = kPathNotChanged;
  const bool removed = std::any_of(
      previous_path.begin(), previous_path.end(), [&](const internal::Path &p) {
        return current_path.find(p) == current_path.end();
      });
  if (removed) {
    path_changed = kPathRemoved;
  }
  (void)std::any_of(current_path.cbegin(), current_path.cend(),
                    [&](const internal::Path &p) -> bool {
                      bool changed = false;
                      const auto find = previous_path.find(p);
                      const bool added_cond = (find == previous_path.end());
                      if (added_cond) {
                        changed = true;
                        path_changed = kPathAdded;
                      } else {
                        const bool updated_cond =
                            (p.GetLastWriteTimestamp() >
                             find->GetLastWriteTimestamp());
                        if (updated_cond) {
                          changed = true;
                          path_changed = kPathUpdated;
                        } else {
                          changed = false;
                        }
                      }
                      return changed;
                    });
  return path_changed;
}

}; // namespace buildcc::internal

#endif
