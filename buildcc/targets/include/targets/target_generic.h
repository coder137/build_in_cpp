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

#ifndef TARGETS_TARGET_GENERIC_H_
#define TARGETS_TARGET_GENERIC_H_

#include <algorithm>
#include <optional>

#include "target/target.h"
#include "toolchain/toolchain.h"

#include "target_gcc.h"
#include "target_mingw.h"
#include "target_msvc.h"

namespace buildcc {

static std::initializer_list<SyncOption> kGenericCopyOptions = {
    SyncOption::CommonCompileFlags, SyncOption::PchCompileFlags,
    SyncOption::PchObjectFlags,     SyncOption::AsmCompileFlags,
    SyncOption::CCompileFlags,      SyncOption::CppCompileFlags,
    SyncOption::LinkFlags,
};

class GenericConfig : ConfigInterface<GenericConfig> {
public:
  static TargetConfig Generic(TargetType type, ToolchainId id) {
    TargetConfig config;
    switch (type) {
    case TargetType::Executable:
      config = Executable(id);
      break;
    case TargetType::StaticLibrary:
      config = StaticLib(id);
      break;
    case TargetType::DynamicLibrary:
      config = DynamicLib(id);
      break;
    default:
      env::assert_fatal<false>("Target Type not supported");
      break;
    }

    return config;
  }

  static TargetConfig Executable(ToolchainId id) {
    return DefaultGenericExecutable(id);
  }
  static TargetConfig StaticLib(ToolchainId id) {
    return DefaultGenericStaticLib(id);
  }
  static TargetConfig DynamicLib(ToolchainId id) {
    return DefaultGenericDynamicLib(id);
  }

private:
  static TargetConfig DefaultGenericExecutable(ToolchainId id) {
    TargetConfig config;
    switch (id) {
    case ToolchainId::Gcc:
      config = GccConfig::Executable();
      break;
    case ToolchainId::Msvc:
      config = MsvcConfig::Executable();
      break;
    case ToolchainId::MinGW:
      config = MingwConfig::Executable();
      break;
    case ToolchainId::Clang:
    default:
      env::assert_fatal<false>("Compiler ID not supported");
      break;
    }

    return config;
  }

  static TargetConfig DefaultGenericStaticLib(ToolchainId id) {
    TargetConfig config;
    switch (id) {
    case ToolchainId::Gcc:
      config = GccConfig::StaticLib();
      break;
    case ToolchainId::Msvc:
      config = MsvcConfig::StaticLib();
      break;
    case ToolchainId::MinGW:
      config = MingwConfig::StaticLib();
      break;
    case ToolchainId::Clang:
    default:
      env::assert_fatal<false>("Compiler ID not supported");
      break;
    }

    return config;
  }

  static TargetConfig DefaultGenericDynamicLib(ToolchainId id) {
    TargetConfig config;
    switch (id) {
    case ToolchainId::Gcc:
      config = GccConfig::DynamicLib();
      break;
    case ToolchainId::Msvc:
      config = MsvcConfig::DynamicLib();
      break;
    case ToolchainId::MinGW:
      config = MingwConfig::DynamicLib();
      break;
    case ToolchainId::Clang:
    default:
      env::assert_fatal<false>("Compiler ID not supported");
      break;
    }

    return config;
  }
};

class ExecutableTarget_generic : public BaseTarget {
public:
  ExecutableTarget_generic(const std::string &name,
                           const BaseToolchain &toolchain, const TargetEnv &env,
                           const std::optional<TargetConfig> &config = {})
      : Target(name, TargetType::Executable, toolchain, env,
               config.value_or(GenericConfig::Executable(toolchain.GetId()))) {
    switch (toolchain.GetId()) {
    case ToolchainId::Gcc:
      Copy(ExecutableTarget_gcc(name, toolchain, env), kGenericCopyOptions);
      break;
    case ToolchainId::Msvc:
      Copy(ExecutableTarget_msvc(name, toolchain, env), kGenericCopyOptions);
      break;
    case ToolchainId::MinGW:
      Copy(ExecutableTarget_mingw(name, toolchain, env), kGenericCopyOptions);
      break;
    case ToolchainId::Clang:
    default:
      env::assert_fatal<false>("Compiler ID not supported");
      break;
    }
  }
  ~ExecutableTarget_generic() {}
};

class StaticTarget_generic : public BaseTarget {
public:
  StaticTarget_generic(const std::string &name, const BaseToolchain &toolchain,
                       const TargetEnv &env,
                       const std::optional<TargetConfig> &config = {})
      : Target(name, TargetType::StaticLibrary, toolchain, env,
               config.value_or(GenericConfig::StaticLib(toolchain.GetId()))) {
    switch (toolchain.GetId()) {
    case ToolchainId::Gcc:
      Copy(StaticTarget_gcc(name, toolchain, env), kGenericCopyOptions);
      break;
    case ToolchainId::Msvc:
      Copy(StaticTarget_msvc(name, toolchain, env), kGenericCopyOptions);
      break;
    case ToolchainId::MinGW:
      Copy(StaticTarget_mingw(name, toolchain, env), kGenericCopyOptions);
      break;
    case ToolchainId::Clang:
    default:
      env::assert_fatal<false>("Compiler ID not supported");
      break;
    }
  }
};

class DynamicTarget_generic : public BaseTarget {
public:
  DynamicTarget_generic(const std::string &name, const BaseToolchain &toolchain,
                        const TargetEnv &env,
                        const std::optional<TargetConfig> &config = {})
      : Target(name, TargetType::DynamicLibrary, toolchain, env,
               config.value_or(GenericConfig::DynamicLib(toolchain.GetId()))) {
    switch (toolchain.GetId()) {
    case ToolchainId::Gcc:
      Copy(DynamicTarget_gcc(name, toolchain, env), kGenericCopyOptions);
      break;
    case ToolchainId::Msvc:
      Copy(DynamicTarget_msvc(name, toolchain, env), kGenericCopyOptions);
      break;
    case ToolchainId::MinGW:
      Copy(DynamicTarget_mingw(name, toolchain, env), kGenericCopyOptions);
      break;
    case ToolchainId::Clang:
    default:
      env::assert_fatal<false>("Compiler ID not supported");
      break;
    }
  }
};

class Target_generic : public BaseTarget {
public:
  Target_generic(const std::string &name, TargetType type,
                 const BaseToolchain &toolchain, const TargetEnv &env,
                 const std::optional<TargetConfig> &config = {})
      : Target(
            name, type, toolchain, env,
            config.value_or(GenericConfig::Generic(type, toolchain.GetId()))) {
    switch (type) {
    case TargetType::Executable:
      Copy(ExecutableTarget_generic(name, toolchain, env), kGenericCopyOptions);
      break;
    case TargetType::StaticLibrary:
      Copy(StaticTarget_generic(name, toolchain, env), kGenericCopyOptions);
      break;
    case TargetType::DynamicLibrary:
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
