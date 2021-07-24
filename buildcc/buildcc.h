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

#ifndef BUILDCC_INCLUDE_BUILDCC_H_
#define BUILDCC_INCLUDE_BUILDCC_H_

// clang-format off

// Core persistent environment
#include "env/env.h"
#include "env/assert_fatal.h"
#include "env/logging.h"
#include "env/host_os.h"

// Base
#include "toolchain.h"
#include "target/target.h"

// Specialized Toolchain
#include "toolchain_gcc.h"
#include "toolchain_msvc.h"

// TODO, Add more specialized toolchains here

// Specialized Targets
#include "target_gcc.h"
#include "target_msvc.h"
#include "target_generic.h"
#include "target_custom.h"

// TODO, Add more specialized targets here

// Plugins
#include "clang_compile_commands.h"

// BuildCC Modules
#include "args.h"
#include "register.h"

#endif
