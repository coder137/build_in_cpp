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

#ifndef TARGET_COMMON_FUNCTION_LOCK_H_
#define TARGET_COMMON_FUNCTION_LOCK_H_

#include <string_view>

#include "fmt/format.h"

#include "env/assert_fatal.h"

namespace buildcc {

class FunctionLock {
public:
  void Lock() { lock_ = true; }
  void Unlock() { lock_ = false; }
  bool IsLocked() const { return lock_; }
  void ExpectsUnlock(std::string_view tag) const {
    env::assert_fatal(!lock_,
                      fmt::format("Cannot use {} when lock == true", tag));
  }
  void ExpectsLock(std::string_view tag) const {
    env::assert_fatal(lock_,
                      fmt::format("Cannot use {} when lock == false", tag));
  }

private:
  bool lock_{false};
};

} // namespace buildcc

#endif
