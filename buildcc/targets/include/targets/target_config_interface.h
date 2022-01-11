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

#ifndef TARGETS_TARGET_CONFIG_INTERFACE_H_
#define TARGETS_TARGET_CONFIG_INTERFACE_H_

#include "target/target.h"

#include <utility>

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
//   virtual TargetConfig Executable() const = 0;
//   virtual TargetConfig StaticLib() const = 0;
//   virtual TargetConfig DynamicLib() const = 0;
// };

// * Instead of Dynamic Polymorphism we use Static Polymorphism using Templates

// CRTP Static Polymorphism interface
// NOTE, Every specialized `<Toolchain>Config` should derive from this
// For example, GccConfig :: public ConfigInterface<GccConfig>
template <typename T> class ConfigInterface {
public:
  template <typename... Args> static TargetConfig Generic(Args &&...args) {
    return T::Generic(std::forward<Args>(args)...);
  }

  template <typename... Args> static TargetConfig Executable(Args &&...args) {
    return T::Executable(std::forward<Args>(args)...);
  }

  template <typename... Args> static TargetConfig StaticLib(Args &&...args) {
    return T::StaticLib(std::forward<Args>(args)...);
  }

  template <typename... Args> static TargetConfig DynamicLib(Args &&...args) {
    return T::DynamicLib(std::forward<Args>(args)...);
  }

  // TODO, Add more functions according to `TargetType`
};

} // namespace buildcc

#endif
