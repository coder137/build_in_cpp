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

#ifndef TARGETS_TARGET_GENERIC_H_
#define TARGETS_TARGET_GENERIC_H_

#include <algorithm>
#include <optional>

#include "target_gcc.h"
#include "target_msvc.h"

namespace buildcc {

static std::initializer_list<base::Target::CopyOption> kGenericCopyOptions = {
    base::Target::CopyOption::CommonCompileFlags,
    base::Target::CopyOption::PchCompileFlags,
    base::Target::CopyOption::PchObjectFlags,
    base::Target::CopyOption::AsmCompileFlags,
    base::Target::CopyOption::CCompileFlags,
    base::Target::CopyOption::CppCompileFlags,
    base::Target::CopyOption::LinkFlags,
};

class GenericConfig : ConfigInterface<GenericConfig> {
public:
  static base::Target::Config Generic(base::Target::Type type,
                                      base::Toolchain::Id id) {
    base::Target::Config config;
    switch (type) {
    case base::Target::Type::Executable:
      config = Executable(id);
      break;
    case base::Target::Type::StaticLibrary:
      config = StaticLib(id);
      break;
    case base::Target::Type::DynamicLibrary:
      config = DynamicLib(id);
      break;
    default:
      env::assert_fatal<false>("Target Type not supported");
      break;
    }

    return config;
  }

  static base::Target::Config Executable(base::Toolchain::Id id) {
    return DefaultGenericExecutable(id);
  }
  static base::Target::Config StaticLib(base::Toolchain::Id id) {
    return DefaultGenericStaticLib(id);
  }
  static base::Target::Config DynamicLib(base::Toolchain::Id id) {
    return DefaultGenericDynamicLib(id);
  }

private:
  static base::Target::Config DefaultGenericExecutable(base::Toolchain::Id id) {
    base::Target::Config config;
    switch (id) {
    case base::Toolchain::Id::Gcc:
      config = GccConfig::Executable();
      break;
    case base::Toolchain::Id::Msvc:
      config = MsvcConfig::Executable();
      break;
    case base::Toolchain::Id::Clang:
    case base::Toolchain::Id::MinGW:
    default:
      env::assert_fatal<false>("Compiler ID not supported");
      break;
    }

    return config;
  }

  static base::Target::Config DefaultGenericStaticLib(base::Toolchain::Id id) {
    base::Target::Config config;
    switch (id) {
    case base::Toolchain::Id::Gcc:
      config = GccConfig::StaticLib();
      break;
    case base::Toolchain::Id::Msvc:
      config = MsvcConfig::StaticLib();
      break;
    case base::Toolchain::Id::Clang:
    case base::Toolchain::Id::MinGW:
    default:
      env::assert_fatal<false>("Compiler ID not supported");
      break;
    }

    return config;
  }

  static base::Target::Config DefaultGenericDynamicLib(base::Toolchain::Id id) {
    base::Target::Config config;
    switch (id) {
    case base::Toolchain::Id::Gcc:
      config = GccConfig::DynamicLib();
      break;
    case base::Toolchain::Id::Msvc:
      config = MsvcConfig::DynamicLib();
      break;
    case base::Toolchain::Id::Clang:
    case base::Toolchain::Id::MinGW:
    default:
      env::assert_fatal<false>("Compiler ID not supported");
      break;
    }

    return config;
  }
};

class ExecutableTarget_generic : public base::Target {
public:
  ExecutableTarget_generic(const std::string &name,
                           const base::Toolchain &toolchain, const Env &env,
                           const std::optional<Config> &config = {})
      : Target(name, base::Target::Type::Executable, toolchain, env,
               config.value_or(GenericConfig::Executable(toolchain.GetId()))) {
    switch (toolchain.GetId()) {
    case base::Toolchain::Id::Gcc:
      Copy(ExecutableTarget_gcc(name, toolchain, env), kGenericCopyOptions);
      break;
    case base::Toolchain::Id::Msvc:
      Copy(ExecutableTarget_msvc(name, toolchain, env), kGenericCopyOptions);
      break;
    case base::Toolchain::Id::Clang:
    case base::Toolchain::Id::MinGW:
    default:
      env::assert_fatal<false>("Compiler ID not supported");
      break;
    }
  }
  ~ExecutableTarget_generic() {}
};

class StaticTarget_generic : public base::Target {
public:
  StaticTarget_generic(const std::string &name,
                       const base::Toolchain &toolchain, const Env &env,
                       const std::optional<Config> &config = {})
      : Target(name, base::Target::Type::StaticLibrary, toolchain, env,
               config.value_or(GenericConfig::StaticLib(toolchain.GetId()))) {
    switch (toolchain.GetId()) {
    case base::Toolchain::Id::Gcc:
      Copy(StaticTarget_gcc(name, toolchain, env), kGenericCopyOptions);
      break;
    case base::Toolchain::Id::Msvc:
      Copy(StaticTarget_msvc(name, toolchain, env), kGenericCopyOptions);
      break;
    case base::Toolchain::Id::Clang:
    case base::Toolchain::Id::MinGW:
    default:
      env::assert_fatal<false>("Compiler ID not supported");
      break;
    }
  }
};

class DynamicTarget_generic : public base::Target {
public:
  DynamicTarget_generic(const std::string &name,
                        const base::Toolchain &toolchain, const Env &env,
                        const std::optional<Config> &config = {})
      : Target(name, base::Target::Type::DynamicLibrary, toolchain, env,
               config.value_or(GenericConfig::DynamicLib(toolchain.GetId()))) {
    switch (toolchain.GetId()) {
    case base::Toolchain::Id::Gcc:
      Copy(DynamicTarget_gcc(name, toolchain, env), kGenericCopyOptions);
      break;
    case base::Toolchain::Id::Msvc:
      Copy(DynamicTarget_msvc(name, toolchain, env), kGenericCopyOptions);
      break;
    case base::Toolchain::Id::Clang:
    case base::Toolchain::Id::MinGW:
    default:
      env::assert_fatal<false>("Compiler ID not supported");
      break;
    }
  }
};

class Target_generic : public base::Target {
public:
  Target_generic(const std::string &name, base::Target::Type type,
                 const base::Toolchain &toolchain, const Env &env,
                 const std::optional<Config> &config = {})
      : Target(
            name, type, toolchain, env,
            config.value_or(GenericConfig::Generic(type, toolchain.GetId()))) {
    switch (type) {
    case base::Target::Type::Executable:
      Copy(ExecutableTarget_generic(name, toolchain, env), kGenericCopyOptions);
      break;
    case base::Target::Type::StaticLibrary:

      Copy(StaticTarget_generic(name, toolchain, env), kGenericCopyOptions);
      break;
    case base::Target::Type::DynamicLibrary:
      Copy(DynamicTarget_generic(name, toolchain, env), kGenericCopyOptions);
      break;
    default:
      env::assert_fatal<false>("Compiler ID not supported");
      break;
    }
  }
};

} // namespace buildcc

#endif
