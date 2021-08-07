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

#include "target_gcc.h"
#include "target_msvc.h"

#include "target_constants.h"
#include "target_utils.h"

// #include "env/host_compiler.h"
// #include "env/host_os.h"

namespace buildcc {

inline void SyncTargetDefaults(base::Target &dest, const base::Target &source) {
  dest.target_ext_ = source.target_ext_;
  dest.obj_ext_ = source.obj_ext_;
  dest.prefix_include_dir_ = source.prefix_include_dir_;
  dest.prefix_lib_dir_ = source.prefix_lib_dir_;
  dest.compile_command_ = source.compile_command_;
  dest.link_command_ = source.link_command_;

  // TODO, Check if other target definitions need to be synced
}

class ExecutableTarget_generic : public base::Target {
public:
  ExecutableTarget_generic(
      const std::string &name, const base::Toolchain &toolchain,
      const std::filesystem::path &target_path_relative_to_root)
      : Target(name, base::TargetType::Executable, toolchain,
               target_path_relative_to_root) {
    std::unique_ptr<base::Target> target;
    switch (toolchain.GetId()) {
    case base::Toolchain::Id::Gcc:
      target = std::make_unique<ExecutableTarget_gcc>(
          name, toolchain, target_path_relative_to_root);
      break;
    case base::Toolchain::Id::Msvc:
      target = std::make_unique<ExecutableTarget_msvc>(
          name, toolchain, target_path_relative_to_root);
      break;
    case base::Toolchain::Id::Clang:
    case base::Toolchain::Id::MinGW:
    default:
      env::assert_fatal(false, "Compiler ID not supported");
      break;
    }
    SyncTargetDefaults(*this, *target);
  }
  ~ExecutableTarget_generic() {}
};

class StaticTarget_generic : public base::Target {
public:
  StaticTarget_generic(
      const std::string &name, const base::Toolchain &toolchain,
      const std::filesystem::path &target_path_relative_to_root)
      : Target(name, base::TargetType::StaticLibrary, toolchain,
               target_path_relative_to_root) {
    std::unique_ptr<base::Target> target;
    switch (toolchain.GetId()) {
    case base::Toolchain::Id::Gcc:
      target = std::make_unique<StaticTarget_gcc>(name, toolchain,
                                                  target_path_relative_to_root);
      break;
    case base::Toolchain::Id::Msvc:
      target = std::make_unique<StaticTarget_msvc>(
          name, toolchain, target_path_relative_to_root);
      break;
    case base::Toolchain::Id::Clang:
    case base::Toolchain::Id::MinGW:
    default:
      env::assert_fatal(false, "Compiler ID not supported");
      break;
    }
    SyncTargetDefaults(*this, *target);
  }
};

class DynamicTarget_generic : public base::Target {
public:
  DynamicTarget_generic(
      const std::string &name, const base::Toolchain &toolchain,
      const std::filesystem::path &target_path_relative_to_root)
      : Target(name, base::TargetType::DynamicLibrary, toolchain,
               target_path_relative_to_root) {
    std::unique_ptr<base::Target> target;
    switch (toolchain.GetId()) {
    case base::Toolchain::Id::Gcc:
      target = std::make_unique<DynamicTarget_gcc>(
          name, toolchain, target_path_relative_to_root);
      break;
    case base::Toolchain::Id::Msvc:
      target = std::make_unique<DynamicTarget_msvc>(
          name, toolchain, target_path_relative_to_root);
      break;
    case base::Toolchain::Id::Clang:
    case base::Toolchain::Id::MinGW:
    default:
      env::assert_fatal(false, "Compiler ID not supported");
      break;
    }
    SyncTargetDefaults(*this, *target);
  }
};

class Target_generic : public base::Target {
public:
  Target_generic(const std::string &name, base::TargetType type,
                 const base::Toolchain &toolchain,
                 const std::filesystem::path &target_path_relative_to_root)
      : Target(name, type, toolchain, target_path_relative_to_root) {
    std::unique_ptr<base::Target> target;
    switch (type) {
    case base::TargetType::Executable:
      target = std::make_unique<ExecutableTarget_generic>(
          name, toolchain, target_path_relative_to_root);
      break;
    case base::TargetType::StaticLibrary:
      target = std::make_unique<StaticTarget_generic>(
          name, toolchain, target_path_relative_to_root);
      break;
    case base::TargetType::DynamicLibrary:
      target = std::make_unique<DynamicTarget_generic>(
          name, toolchain, target_path_relative_to_root);
      break;
    default:
      env::assert_fatal(false, "Compiler ID not supported");
      break;
    }
    SyncTargetDefaults(*this, *target);
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
};

} // namespace buildcc

#endif
