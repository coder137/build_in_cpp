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

#ifndef BUILDEXE_TOOLCHAIN_SETUP_H_
#define BUILDEXE_TOOLCHAIN_SETUP_H_

#include "buildcc.h"

namespace buildcc {

void find_toolchain_verify(BaseToolchain &toolchain);
void host_toolchain_verify(const BaseToolchain &toolchain);

} // namespace buildcc

#endif
