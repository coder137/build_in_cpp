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

#ifndef TARGETS_TARGET_CONFIG_INTERFACE_H_
#define TARGETS_TARGET_CONFIG_INTERFACE_H_

#include "target/target.h"

namespace buildcc {

// USAGE:
// Derive from this class and implement specialized configs for specialized
// target/toolchains

// NOTE: `virtual` has only been added here for an interface pattern

// LIMITATIONS:
// We would rather be able to do this `Derived::Executable()`
// However `virtual` cannot be added with `static` hence we need to initialize
// the class before usage
// For example: `Derived().Executable()`
// This happens because the `virtual` and `static` keyword have conflicting
// definitions in C++ when associated with classes
// `virtual` - Does not relate to a class, only to the instance
// `static` - Does not relate to an instance, only to the class

// OUR USECASE for both `virtual` and `static`:
// We would like to have the interface pattern using `virtual`
// The `static` keyword would be used to associate the specialization to a class
// rather than an instance
// This would enable `Derived::Executable()` with interfaces

// NOTE: This implementation has been kept to show current limitations
// class ConfigInterface {
// public:
//   virtual base::Target::Config Executable() const = 0;
//   virtual base::Target::Config StaticLib() const = 0;
//   virtual base::Target ::Config DynamicLib() const = 0;
// };

// Compile-Time interface check
template <typename T> class ConfigInterface {
public:
  static base::Target::Config Executable() { return T::Executable(); }
  static base::Target::Config StaticLib() { return T::StaticLib(); }
  static base::Target ::Config DynamicLib() { return T::DynamicLib(); }
  // TODO, Add more functions according to `Target::TargetType`
};

} // namespace buildcc

#endif
