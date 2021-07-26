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

#ifndef TARGETS_TARGET_MSVC_H_
#define TARGETS_TARGET_MSVC_H_

#include "target/target.h"

#include "target_constants.h"

// TODO, Combine all of these into Target_msvc
namespace buildcc {

class ExecutableTarget_msvc : public base::Target {
public:
  ExecutableTarget_msvc(
      const std::string &name, const base::Toolchain &toolchain,
      const std::filesystem::path &target_path_relative_to_root)
      : Target(name, base::TargetType::Executable, toolchain,
               target_path_relative_to_root) {
    prefix_include_dir_ = kMsvcPrefixIncludeDir;
    prefix_lib_dir_ = kMsvcPrefixLibDir;
  }

private:
  std::string_view CompileCommand() const override {
    return kMsvcCompileCommand;
  }
  std::string_view Link() const override { return kMsvcExecutableLinkCommand; }
};

class StaticTarget_msvc : public base::Target {
public:
  StaticTarget_msvc(const std::string &name, const base::Toolchain &toolchain,
                    const std::filesystem::path &target_path_relative_to_root)
      : Target(name, base::TargetType::StaticLibrary, toolchain,
               target_path_relative_to_root) {
    prefix_include_dir_ = kMsvcPrefixIncludeDir;
    prefix_lib_dir_ = kMsvcPrefixLibDir;
  }

private:
  std::string_view CompileCommand() const override {
    return kMsvcCompileCommand;
  }
  std::string_view Link() const override { return kMsvcStaticLibLinkCommand; }
};

class DynamicTarget_msvc : public base::Target {
public:
  DynamicTarget_msvc(const std::string &name, const base::Toolchain &toolchain,
                     const std::filesystem::path &target_path_relative_to_root)
      : Target(name, base::TargetType::DynamicLibrary, toolchain,
               target_path_relative_to_root) {
    prefix_include_dir_ = kMsvcPrefixIncludeDir;
    prefix_lib_dir_ = kMsvcPrefixLibDir;
  }

private:
  std::string_view CompileCommand() const override {
    return kMsvcCompileCommand;
  }
  std::string_view Link() const override { return kMsvcDynamicLibLinkCommand; }
};

} // namespace buildcc

#endif
