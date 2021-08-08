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

#include "target/target.h"

// TODO, Combine all of these into Target_gcc
namespace buildcc {

// Extensions
constexpr std::string_view kGccStaticLibExt = ".a";
constexpr std::string_view kGccDynamicLibExt = ".so";

// GCC
constexpr std::string_view kGccPrefixIncludeDir = "-I";
constexpr std::string_view kGccPrefixLibDir = "-L";
constexpr std::string_view kGccGenericCompileCommand =
    "{compiler} {preprocessor_flags} {include_dirs} {common_compile_flags} "
    "{compile_flags} -o {output} -c {input}";
constexpr std::string_view kGccExecutableLinkCommand =
    "{cpp_compiler} {link_flags} {compiled_sources} -o {output} "
    "{lib_dirs} {lib_deps}";
constexpr std::string_view kGccStaticLibLinkCommand =
    "{archiver} rcs {output} {compiled_sources}";
constexpr std::string_view kGccDynamicLibCompileCommand =
    "{compiler} {preprocessor_flags} {include_dirs} {common_compile_flags} "
    "{compile_flags} -fpic -o {output} -c {input}";
constexpr std::string_view kGccDynamicLibLinkCommand =
    "{cpp_compiler} -shared {link_flags} {compiled_sources} -o {output}";

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
               target_path_relative_to_root) {
    target_ext_ = kGccStaticLibExt;
    link_command_ = kGccStaticLibLinkCommand;
  }
};

class DynamicTarget_gcc : public base::Target {
public:
  DynamicTarget_gcc(const std::string &name, const base::Toolchain &toolchain,
                    const std::filesystem::path &target_path_relative_to_root)
      : Target(name, base::TargetType::DynamicLibrary, toolchain,
               target_path_relative_to_root) {
    target_ext_ = kGccDynamicLibExt;
    compile_command_ = kGccDynamicLibCompileCommand;
    link_command_ = kGccDynamicLibLinkCommand;
  }
};

} // namespace buildcc

#endif
