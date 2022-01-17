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

#ifndef TARGET_COMMON_TARGET_STATE_H_
#define TARGET_COMMON_TARGET_STATE_H_

#include "target/common/target_file_ext.h"

namespace buildcc {

// TODO, Seperate TargetState into lock_ and other internal boolean variables
// NOTE, This is because apart from lock_ every other parameter is updated
// during `Target::Build`
// TargetInfo does not have a `Build` method, it is only meant to hold
// information
struct TargetState {
  void SetSourceState(TargetFileExt file_extension);
  void SetPch();
  void SetLock();

  void ExpectsUnlock() const;
  void ExpectsLock() const;
  // TODO, IsLocked

  bool ContainsPch() const { return contains_pch_; }

  // TODO, Make these private getters
  bool contains_asm{false};
  bool contains_c{false};
  bool contains_cpp{false};
  bool build{false};
  bool lock{false};

private:
  bool contains_pch_{false};
};

} // namespace buildcc

#endif
