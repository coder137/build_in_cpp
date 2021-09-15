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

#ifndef TARGET_BUILDER_INTERFACE_H_
#define TARGET_BUILDER_INTERFACE_H_

#include <algorithm>
#include <unordered_set>

#include "target/path.h"
#include "target/util.h"

namespace buildcc::base {

// TODO, 1. Consider updating Recheck* APIs - do not modify internal `dirty_`
// flag
// TODO, 2. Consider removing dependency on target/util.h
// TODO, 3. Consider making Recheck* APIs free namespaced functions instead of
// only within the scope of BuilderInterfaces (See TODO 1. and 2. first)
class BuilderInterface {

public:
  virtual void Build() = 0;

protected:
  template <typename T>
  void RecheckChanged(
      const T &previous, const T &current,
      const std::function<void(void)> &callback = []() {}) {
    if (dirty_) {
      return;
    }

    if (previous != current) {
      if (callback) {
        callback();
      }
      dirty_ = true;
    }
  }

  void RecheckPaths(
      const internal::path_unordered_set &previous_path,
      const internal::path_unordered_set &current_path,
      const std::function<void(void)> &path_removed_cb = []() {},
      const std::function<void(void)> &path_added_cb = []() {},
      const std::function<void(void)> &path_updated_cb = []() {}) {
    if (dirty_) {
      return;
    }

    // * Old path is removed
    const bool removed =
        std::any_of(previous_path.begin(), previous_path.end(),
                    [&](const internal::Path &p) {
                      return current_path.find(p) == current_path.end();
                    });
    if (removed) {
      if (path_removed_cb) {
        path_removed_cb();
      }
      dirty_ = true;
      return;
    }

    for (const auto &path : current_path) {
      auto iter = previous_path.find(path);

      if (iter == previous_path.end()) {
        // * New path added
        if (path_added_cb) {
          path_added_cb();
        }
        dirty_ = true;
      } else {
        // * Path is updated
        if (path.GetLastWriteTimestamp() > iter->GetLastWriteTimestamp()) {
          if (path_updated_cb) {
            path_updated_cb();
          }
          dirty_ = true;
        } else {
          // * Do nothing
        }
      }

      if (dirty_) {
        break;
      }
    }
  }

private:
  virtual bool Store() = 0;

protected:
  bool dirty_{false};
};

} // namespace buildcc::base

#endif
