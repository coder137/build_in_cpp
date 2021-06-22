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

#ifndef TARGETS_TARGET_GCC_H_
#define TARGETS_TARGET_GCC_H_

#include "target.h"

#include "target_constants.h"

// TODO, Combine all of these into Target_gcc
namespace buildcc {

class ExecutableTarget_gcc : public base::Target {
public:
  ExecutableTarget_gcc(
      const std::string &name, const base::Toolchain &toolchain,
      const std::filesystem::path &target_path_relative_to_root)
      : Target(name, base::TargetType::Executable, toolchain,
               target_path_relative_to_root) {}
};

class StaticTarget_gcc : public base::Target {
public:
  StaticTarget_gcc(const std::string &name, const base::Toolchain &toolchain,
                   const std::filesystem::path &target_path_relative_to_root)
      : Target(name, base::TargetType::StaticLibrary, toolchain,
               target_path_relative_to_root) {}

private:
  std::string_view Link() const override { return kGccStaticLibLinkCommand; }
};

class DynamicTarget_gcc : public base::Target {
public:
  DynamicTarget_gcc(const std::string &name, const base::Toolchain &toolchain,
                    const std::filesystem::path &target_path_relative_to_root)
      : Target(name, base::TargetType::DynamicLibrary, toolchain,
               target_path_relative_to_root) {}

private:
  std::string_view CompileCommand() const override {
    return kGccDynamicLibCompileCommand;
  }
  std::string_view Link() const override { return kGccDynamicLibLinkCommand; }
};

} // namespace buildcc

#endif
