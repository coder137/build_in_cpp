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

#ifndef TARGET_INTERFACE_BUILDER_INTERFACE_H_
#define TARGET_INTERFACE_BUILDER_INTERFACE_H_

#include <algorithm>
#include <functional>
#include <unordered_set>

#include "target/base/path.h"
#include "target/common/util.h"

namespace buildcc::base {

// TODO, 1. Consider updating Recheck* APIs - do not modify internal `dirty_`
// flag
// TODO, 2. Consider removing dependency on target/common/util.h
// TODO, 3. Consider making Recheck* APIs free namespaced functions instead of
// only within the scope of BuilderInterfaces (See TODO 1. and 2. first)
class BuilderInterface {

public:
  virtual void Build() = 0;

  const std::string &GetUniqueId() const { return unique_id_; }

protected:
  template <typename T>
  void RecheckChanged(
      const T &previous, const T &current,
      const std::function<void(void)> &callback = []() {}) {
    ASSERT_FATAL(callback, "Bad function: callback");

    if (dirty_) {
      return;
    }

    if (previous != current) {
      callback();
      dirty_ = true;
    }
  }

  void RecheckPaths(
      const internal::path_unordered_set &previous_path,
      const internal::path_unordered_set &current_path,
      const std::function<void(void)> &path_removed_cb = []() {},
      const std::function<void(void)> &path_added_cb = []() {},
      const std::function<void(void)> &path_updated_cb = []() {}) {
    ASSERT_FATAL(path_removed_cb, "Bad function: path_removed_cb");
    ASSERT_FATAL(path_added_cb, "Bad function: path_added_cb");
    ASSERT_FATAL(path_updated_cb, "Bad function: path_updated_cb");

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
      path_removed_cb();
      dirty_ = true;
      return;
    }

    dirty_ = std::any_of(
        current_path.cbegin(), current_path.cend(),
        [&](const internal::Path &p) -> bool {
          bool dirty = false;
          const auto find = previous_path.find(p);
          const bool added_cond = (find == previous_path.end());
          if (added_cond) {
            path_added_cb();
            dirty = true;
          } else {
            const bool updated_cond =
                (p.GetLastWriteTimestamp() > find->GetLastWriteTimestamp());
            if (updated_cond) {
              path_updated_cb();
              dirty = true;
            } else {
              dirty = false;
            }
          }
          return dirty;
        });
  }

private:
  virtual bool Store() = 0;

protected:
  bool dirty_{false};
  std::string unique_id_;
};

} // namespace buildcc::base

#endif
