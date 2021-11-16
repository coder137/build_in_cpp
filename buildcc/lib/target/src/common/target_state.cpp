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

#include "target/common/target_state.h"

namespace buildcc::base {

void TargetState::SetSourceState(TargetFileExt file_extension) {
  switch (file_extension) {
  case TargetFileExt::Asm:
    contains_asm = true;
    break;
  case TargetFileExt::C:
    contains_c = true;
    break;
  case TargetFileExt::Cpp:
    contains_cpp = true;
    break;
  case TargetFileExt::Header:
  case TargetFileExt::Invalid:
  default:
    break;
  }
}

} // namespace buildcc::base
