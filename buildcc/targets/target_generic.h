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

#include "target.h"

#include "target_constants.h"

#include "host_os.h"

namespace buildcc {

class Target_generic : public base::Target {
public:
  Target_generic(const std::string &name, base::TargetType type,
                 const base::Toolchain &toolchain,
                 const std::filesystem::path &target_path_relative_to_root)
      : Target(Name(name, type, toolchain), type, toolchain,
               target_path_relative_to_root) {
    Initialize();
  }

  static std::string_view Extension(base::TargetType type,
                                    base::Toolchain::Id id) {
    switch (id) {
    // Toolchain ID: GCC
    // Target Type: Executable, StaticLib, DynamicLib
    // OS: Linux == Macos, Windows
    case base::Toolchain::Id::Gcc: {
      switch (type) {
      case base::TargetType::StaticLibrary:
        return kGccStaticLibExt;
      case base::TargetType::DynamicLibrary:
        if constexpr (env::is_win()) {
          return kWinDynamicLibExt;
        } else {
          return kUnixDynamicLibExt;
        }
      case base::TargetType::Executable:
        if constexpr (env::is_win()) {
          return kWinExecutableExt;
        }
      default:
        break;
      }
    } break;
    // Toolchain ID: MSVC
    // Target Type: Executable, StaticLib, DynamicLib
    // OS: Windows
    case base::Toolchain::Id::Msvc:
      switch (type) {
      case base::TargetType::StaticLibrary:
        return kWinStaticLibExt;
      case base::TargetType::DynamicLibrary:
        return kWinDynamicLibExt;
      case base::TargetType::Executable:
        return kWinExecutableExt;
      default:
        break;
      }
      break;
    default:
      break;
    }
    return "";
  }

private:
  std::string_view CompileCommand() const override {
    switch (GetToolchain().GetId()) {
    case base::Toolchain::Id::Gcc:
      return GccCompileCommand();
      break;
    case base::Toolchain::Id::Msvc:
      return MsvcCompileCommand();
      break;
    default:
      break;
    }
    return "";
  }
  std::string_view Link() const override {
    switch (GetToolchain().GetId()) {
    case base::Toolchain::Id::Gcc:
      return GccLink();
      break;
    case base::Toolchain::Id::Msvc:
      return MsvcLink();
      break;
    default:
      break;
    }
    return "";
  }

  void Initialize() {
    switch (GetToolchain().GetId()) {
    case base::Toolchain::Id::Gcc:
      GccInitialize();
      break;
    case base::Toolchain::Id::Msvc:
      MsvcInitialize();
      break;
    default:
      break;
    }
  }

  // GCC
  void GccInitialize() {
    prefix_include_dir_ = kGccPrefixIncludeDir;
    prefix_lib_dir_ = kGccPrefixLibDir;
  }
  std::string_view GccCompileCommand() const {
    switch (GetTargetType()) {
    case base::TargetType::Executable:
    case base::TargetType::StaticLibrary:
      return kGccGenericCompileCommand;
    case base::TargetType::DynamicLibrary:
      return kGccDynamicLibCompileCommand;
    default:
      break;
    }
    return "";
  }
  std::string_view GccLink() const {
    switch (GetTargetType()) {
    case base::TargetType::Executable:
      return kGccExecutableLinkCommand;
    case base::TargetType::StaticLibrary:
      return kGccStaticLibLinkCommand;
    case base::TargetType::DynamicLibrary:
      return kGccDynamicLibLinkCommand;
    default:
      break;
    }
    return "";
  }

  // MSVC
  void MsvcInitialize() {
    prefix_include_dir_ = kMsvcPrefixIncludeDir;
    prefix_lib_dir_ = kMsvcPrefixLibDir;
    AddCCompileFlag("/nologo");
    AddCppCompileFlag("/nologo");
    AddCppCompileFlag("/EHsc");
    AddLinkFlag("/nologo");
  }
  std::string_view MsvcCompileCommand() const {
    switch (GetTargetType()) {
    case base::TargetType::Executable:
    case base::TargetType::StaticLibrary:
    case base::TargetType::DynamicLibrary:
      return kMsvcCompileCommand;
    default:
      break;
    }
    return "";
  }
  std::string_view MsvcLink() const {
    switch (GetTargetType()) {
    case base::TargetType::Executable:
      return kMsvcExecutableLinkCommand;
    case base::TargetType::StaticLibrary:
      return kMsvcStaticLibLinkCommand;
    case base::TargetType::DynamicLibrary:
      return kMsvcDynamicLibLinkCommand;
    default:
      break;
    }
    return "";
  }

  // OTHER
  std::string Name(const std::string &name, base::TargetType type,
                   const base::Toolchain &toolchain) const {
    return fmt::format("{}{}", name, Extension(type, toolchain.GetId()));
  }
};

} // namespace buildcc

#endif
