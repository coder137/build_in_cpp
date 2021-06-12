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

#ifndef TOOLCHAINS_TOOLCHAIN_MSVC_H_
#define TOOLCHAINS_TOOLCHAIN_MSVC_H_

#include "toolchain.h"

namespace buildcc {

class Toolchain_msvc : public base::Toolchain {
public:
  Toolchain_msvc() : Toolchain("msvc", "cl", "cl", "cl", "lib", "link") {}
  Toolchain_msvc(const Toolchain_msvc &gcc) = delete;
};

} // namespace buildcc

#endif
