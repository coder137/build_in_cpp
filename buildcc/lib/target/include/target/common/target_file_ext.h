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

#ifndef TARGET_COMMON_TARGET_FILE_EXT_H_
#define TARGET_COMMON_TARGET_FILE_EXT_H_

namespace buildcc {

enum class TargetFileExt {
  Asm,     ///< Valid Assembly source extension
  C,       ///< Valid C source extension
  Cpp,     ///< Valid Cpp source extension
  Header,  ///< Valid Header extension
  Invalid, ///< Not a valid C/C++ family extension
};

}

#endif
