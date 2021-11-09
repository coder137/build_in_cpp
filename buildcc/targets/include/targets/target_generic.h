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

class GenericConfig {
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

inline void
CopyTarget(base::Target &dest, base::Target &&src,
           std::initializer_list<base::Target::CopyOption> options) {
  dest.Copy(src, options);
}

class ExecutableTarget_generic : public base::Target {
public:
  ExecutableTarget_generic(
      const std::string &name, const base::Toolchain &toolchain,
      const std::filesystem::path &target_path_relative_to_root)
      : Target(name, base::Target::Type::Executable, toolchain,
               target_path_relative_to_root,
               ConfigInterface<GenericConfig, base::Toolchain::Id>::Executable(
                   toolchain.GetId())) {

    switch (toolchain.GetId()) {
    case base::Toolchain::Id::Gcc:
      CopyTarget(
          *this,
          ExecutableTarget_gcc(name, toolchain, target_path_relative_to_root),
          kGenericCopyOptions);
      break;
    case base::Toolchain::Id::Msvc:
      CopyTarget(
          *this,
          ExecutableTarget_msvc(name, toolchain, target_path_relative_to_root),
          kGenericCopyOptions);
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
  StaticTarget_generic(
      const std::string &name, const base::Toolchain &toolchain,
      const std::filesystem::path &target_path_relative_to_root)
      : Target(name, base::Target::Type::StaticLibrary, toolchain,
               target_path_relative_to_root,
               ConfigInterface<GenericConfig, base::Toolchain::Id>::StaticLib(
                   toolchain.GetId())) {
    switch (toolchain.GetId()) {
    case base::Toolchain::Id::Gcc:
      CopyTarget(
          *this,
          StaticTarget_gcc(name, toolchain, target_path_relative_to_root),
          kGenericCopyOptions);
      break;
    case base::Toolchain::Id::Msvc:
      CopyTarget(
          *this,
          StaticTarget_msvc(name, toolchain, target_path_relative_to_root),
          kGenericCopyOptions);
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
  DynamicTarget_generic(
      const std::string &name, const base::Toolchain &toolchain,
      const std::filesystem::path &target_path_relative_to_root)
      : Target(name, base::Target::Type::DynamicLibrary, toolchain,
               target_path_relative_to_root,
               ConfigInterface<GenericConfig, base::Toolchain::Id>::DynamicLib(
                   toolchain.GetId())) {
    switch (toolchain.GetId()) {
    case base::Toolchain::Id::Gcc:
      CopyTarget(
          *this,
          DynamicTarget_gcc(name, toolchain, target_path_relative_to_root),
          kGenericCopyOptions);
      break;
    case base::Toolchain::Id::Msvc:
      CopyTarget(
          *this,
          DynamicTarget_msvc(name, toolchain, target_path_relative_to_root),
          kGenericCopyOptions);
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
                 const base::Toolchain &toolchain,
                 const std::filesystem::path &target_path_relative_to_root)
      : Target(
            name, type, toolchain, target_path_relative_to_root,
            ConfigInterface<GenericConfig, base::Target::Type,
                            base::Toolchain::Id>::Generic(type,
                                                          toolchain.GetId())) {
    switch (type) {
    case base::Target::Type::Executable:
      CopyTarget(*this,
                 ExecutableTarget_generic(name, toolchain,
                                          target_path_relative_to_root),
                 kGenericCopyOptions);
      break;
    case base::Target::Type::StaticLibrary:

      CopyTarget(
          *this,
          StaticTarget_generic(name, toolchain, target_path_relative_to_root),
          kGenericCopyOptions);
      break;
    case base::Target::Type::DynamicLibrary:
      CopyTarget(
          *this,
          DynamicTarget_generic(name, toolchain, target_path_relative_to_root),
          kGenericCopyOptions);
      break;
    default:
      env::assert_fatal<false>("Compiler ID not supported");
      break;
    }
  }
};

} // namespace buildcc

#endif
