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

#include "targets/target_config_interface.h"

// TODO, Combine all of these into Target_msvc
namespace buildcc {

// MSVC Constants
constexpr const char *const kMsvcExecutableExt = ".exe";
constexpr const char *const kMsvcStaticLibExt = ".lib";
// Why is `kWinDynamicLibExt != .dll` but `.lib` instead?
// See `kMsvcDynamicLibLinkCommand`
// IMPLIB .lib stubs are what is linked during link time
// OUT .dll needs to be present in the executable folder during runtime
constexpr const char *const kMsvcDynamicLibExt = ".lib";

constexpr const char *const kMsvcObjExt = ".obj";
constexpr const char *const kMsvcPchHeaderExt = ".h";
constexpr const char *const kMsvcPchCompileExt = ".pch";

constexpr const char *const kMsvcPrefixIncludeDir = "/I";
constexpr const char *const kMsvcPrefixLibDir = "/LIBPATH:";

constexpr const char *const kMsvcPchCompileCommand =
    "{compiler} {preprocessor_flags} {include_dirs} {common_compile_flags} "
    "/Yc{input} /FI{input} /Fp{output} {pch_compile_flags} {compile_flags} "
    "/Fo{pch_object_output} /c {input_source}";
// TODO, Split this into individual CompileCommands if any differences occur
constexpr const char *const kMsvcCompileCommand =
    "{compiler} {preprocessor_flags} {include_dirs} {common_compile_flags} "
    "{pch_object_flags} {compile_flags} /Fo{output} /c {input}";
constexpr const char *const kMsvcExecutableLinkCommand =
    "{linker} {link_flags} {lib_dirs} /OUT:{output} {lib_deps} "
    "{compiled_sources} {pch_object_output}";
constexpr const char *const kMsvcStaticLibLinkCommand =
    "{archiver} {link_flags} /OUT:{output} {compiled_sources} "
    "{pch_object_output}";
constexpr const char *const kMsvcDynamicLibLinkCommand =
    "{linker} /DLL {link_flags} /OUT:{output}.dll /IMPLIB:{output} "
    "{compiled_sources} {pch_object_output}";

class MsvcConfig {
public:
  static base::Target::Config Executable() {
    return DefaultMsvcConfig(kMsvcExecutableExt, kMsvcCompileCommand,
                             kMsvcExecutableLinkCommand);
  }
  static base::Target::Config StaticLib() {
    return DefaultMsvcConfig(kMsvcStaticLibExt, kMsvcCompileCommand,
                             kMsvcStaticLibLinkCommand);
  }
  static base::Target ::Config DynamicLib() {
    return DefaultMsvcConfig(kMsvcDynamicLibExt, kMsvcCompileCommand,
                             kMsvcDynamicLibLinkCommand);
  }

private:
  static base::Target::Config
  DefaultMsvcConfig(const std::string &target_ext,
                    const std::string &compile_command,
                    const std::string &link_command) {
    base::Target::Config config;
    config.target_ext = target_ext;
    config.obj_ext = kMsvcObjExt;
    config.pch_header_ext = kMsvcPchHeaderExt;
    config.pch_compile_ext = kMsvcPchCompileExt;
    config.prefix_include_dir = kMsvcPrefixIncludeDir;
    config.prefix_lib_dir = kMsvcPrefixLibDir;
    config.pch_command = kMsvcPchCompileCommand;
    config.compile_command = compile_command;
    config.link_command = link_command;
    return config;
  }
};

inline void DefaultMsvcOptions(base::Target &target) {
  target.AddCCompileFlag("/nologo");
  target.AddCppCompileFlag("/nologo");
  target.AddCppCompileFlag("/EHsc"); // TODO, Might need to remove this
  target.AddLinkFlag("/nologo");
  target.AddPchObjectFlag(
      fmt::format("/Yu{}", target.GetPchHeaderPath().string()));
  target.AddPchObjectFlag(
      fmt::format("/FI{}", target.GetPchHeaderPath().string()));
  target.AddPchObjectFlag(
      fmt::format("/Fp{}", target.GetPchCompilePath().string()));
}

class ExecutableTarget_msvc : public base::Target {
public:
  ExecutableTarget_msvc(
      const std::string &name, const base::Toolchain &toolchain,
      const std::filesystem::path &target_path_relative_to_root)
      : Target(name, base::Target::Type::Executable, toolchain,
               target_path_relative_to_root,
               ConfigInterface<MsvcConfig>::Executable()) {
    DefaultMsvcOptions(*this);
  }
};

class StaticTarget_msvc : public base::Target {
public:
  StaticTarget_msvc(const std::string &name, const base::Toolchain &toolchain,
                    const std::filesystem::path &target_path_relative_to_root)
      : Target(name, base::Target::Type::StaticLibrary, toolchain,
               target_path_relative_to_root,
               ConfigInterface<MsvcConfig>::StaticLib()) {
    DefaultMsvcOptions(*this);
  }
};

class DynamicTarget_msvc : public base::Target {
public:
  DynamicTarget_msvc(const std::string &name, const base::Toolchain &toolchain,
                     const std::filesystem::path &target_path_relative_to_root)
      : Target(name, base::Target::Type::DynamicLibrary, toolchain,
               target_path_relative_to_root,
               ConfigInterface<MsvcConfig>::DynamicLib()) {
    DefaultMsvcOptions(*this);
  }
};

} // namespace buildcc

#endif
