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

#include "target.h"

namespace buildcc {

// TODO, Later shift this into a constants file
constexpr const char *const kMsvcPrefixIncludeDir = "/I";
constexpr const char *const kMsvcPrefixLibDir = "/LIBPATH:";
constexpr const char *const kMsvcCompileCommand =
    "{compiler} {preprocessor_flags} {include_dirs} {compile_flags} "
    "/Fo{output} /c {input}";

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
  virtual std::string_view CompileCommand() const {
    return kMsvcCompileCommand;
  }
  virtual std::string_view Link() const {
    return "{linker} {link_flags} {lib_dirs} /OUT:{output} {lib_deps} "
           "{compiled_sources}";
  }
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
  virtual std::string_view CompileCommand() const {
    return kMsvcCompileCommand;
  }
  virtual std::string_view Link() const {
    return "{archiver} {link_flags} /OUT:{output} {compiled_sources}";
  }
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
  virtual std::string_view CompileCommand() const {
    return kMsvcCompileCommand;
  }
  virtual std::string_view Link() const {
    return "{linker} /DLL {link_flags} /OUT:{output}.dll /IMPLIB:{output} "
           "{compiled_sources}";
  }
};

} // namespace buildcc

#endif
