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

#ifndef TARGET_COMMON_TARGET_STATE_H_
#define TARGET_COMMON_TARGET_STATE_H_

#include "target/common/target_file_ext.h"

namespace buildcc::base {

struct TargetState {

  void SetSourceState(TargetFileExt file_extension);
  void Lock();

  void ExpectsUnlock() const;
  void ExpectsLock() const;

  // TODO, Make these private getters
  // TODO, Make a setter for pch
  bool contains_pch{false};
  bool contains_asm{false};
  bool contains_c{false};
  bool contains_cpp{false};
  bool build{false};
  bool lock{false};
};

} // namespace buildcc::base

#endif
