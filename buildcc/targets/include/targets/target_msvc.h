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

// MSVC Constants
constexpr std::string_view kMsvcExecutableExt = ".exe";
constexpr std::string_view kMsvcStaticLibExt = ".lib";
// Why is `kWinDynamicLibExt != .dll` but `.lib` instead?
// See `kMsvcDynamicLibLinkCommand`
// IMPLIB .lib stubs are what is linked during link time
// OUT .dll needs to be present in the executable folder during runtime
constexpr std::string_view kMsvcDynamicLibExt = ".lib";

constexpr std::string_view kMsvcObjExt = ".obj";
constexpr std::string_view kMsvcPrefixIncludeDir = "/I";
constexpr std::string_view kMsvcPrefixLibDir = "/LIBPATH:";
// TODO, Split this into individual CompileCommands if any differences occur
constexpr std::string_view kMsvcCompileCommand =
    "{compiler} {preprocessor_flags} {include_dirs} {compile_flags} "
    "/Fo{output} /c {input}";
constexpr std::string_view kMsvcExecutableLinkCommand =
    "{linker} {link_flags} {lib_dirs} /OUT:{output} {lib_deps} "
    "{compiled_sources}";
constexpr std::string_view kMsvcStaticLibLinkCommand =
    "{archiver} {link_flags} /OUT:{output} {compiled_sources}";
constexpr std::string_view kMsvcDynamicLibLinkCommand =
    "{linker} /DLL {link_flags} /OUT:{output}.dll /IMPLIB:{output} "
    "{compiled_sources}";

inline void DefaultMsvcOptions(base::Target &target) {
  target.obj_ext_ = kMsvcObjExt;
  target.prefix_include_dir_ = kMsvcPrefixIncludeDir;
  target.prefix_lib_dir_ = kMsvcPrefixLibDir;

  target.AddCCompileFlag("/nologo");
  target.AddCppCompileFlag("/nologo");
  target.AddCppCompileFlag("/EHsc"); // TODO, Might need to remove this
  target.AddLinkFlag("/nologo");
}

class ExecutableTarget_msvc : public base::Target {
public:
  ExecutableTarget_msvc(
      const std::string &name, const base::Toolchain &toolchain,
      const std::filesystem::path &target_path_relative_to_root)
      : Target(name, base::TargetType::Executable, toolchain,
               target_path_relative_to_root) {
    target_ext_ = kMsvcExecutableExt;
    compile_command_ = kMsvcCompileCommand;
    link_command_ = kMsvcExecutableLinkCommand;
    DefaultMsvcOptions(*this);
  }
};

class StaticTarget_msvc : public base::Target {
public:
  StaticTarget_msvc(const std::string &name, const base::Toolchain &toolchain,
                    const std::filesystem::path &target_path_relative_to_root)
      : Target(name, base::TargetType::StaticLibrary, toolchain,
               target_path_relative_to_root) {
    target_ext_ = kMsvcStaticLibExt;
    compile_command_ = kMsvcCompileCommand;
    link_command_ = kMsvcStaticLibLinkCommand;
    DefaultMsvcOptions(*this);
  }
};

class DynamicTarget_msvc : public base::Target {
public:
  DynamicTarget_msvc(const std::string &name, const base::Toolchain &toolchain,
                     const std::filesystem::path &target_path_relative_to_root)
      : Target(name, base::TargetType::DynamicLibrary, toolchain,
               target_path_relative_to_root) {
    target_ext_ = kMsvcDynamicLibExt;
    compile_command_ = kMsvcCompileCommand;
    link_command_ = kMsvcDynamicLibLinkCommand;
    DefaultMsvcOptions(*this);
  }
};

} // namespace buildcc

#endif
