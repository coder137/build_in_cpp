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

#include "toolchain/common/file_ext.h"

namespace buildcc {

struct TargetState {
  void BuildCompleted();
  void SetSourceState(FileExt file_extension);
  void SetPch();

  bool IsBuilt() const { return build_; }
  bool ContainsPch() const { return contains_pch_; }
  bool ContainsAsm() const { return contains_asm_; }
  bool ContainsC() const { return contains_c_; }
  bool ContainsCpp() const { return contains_cpp_; }

private:
  bool build_{false};

  bool contains_pch_{false};
  bool contains_asm_{false};
  bool contains_c_{false};
  bool contains_cpp_{false};
};

} // namespace buildcc

#endif
