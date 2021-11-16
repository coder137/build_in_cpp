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

#include "target_config_interface.h"

// TODO, Combine all of these into Target_gcc
namespace buildcc {

// Extensions
constexpr const char *const kGccExecutableExt = "";
constexpr const char *const kGccStaticLibExt = ".a";
constexpr const char *const kGccDynamicLibExt = ".so";

constexpr const char *const kGccObjExt = ".o";
constexpr const char *const kGccPchHeaderExt = ".h";
constexpr const char *const kGccPchCompileExt = ".gch";

// GCC
constexpr const char *const kGccPrefixIncludeDir = "-I";
constexpr const char *const kGccPrefixLibDir = "-L";

constexpr const char *const kGccGenericPchCompileCommand =
    "{compiler} {preprocessor_flags} {include_dirs} {common_compile_flags} "
    "{pch_compile_flags} {compile_flags} -o {output} -c {input}";
constexpr const char *const kGccGenericCompileCommand =
    "{compiler} {preprocessor_flags} {include_dirs} {common_compile_flags} "
    "{pch_object_flags} {compile_flags} -o {output} -c {input}";
constexpr const char *const kGccExecutableLinkCommand =
    "{cpp_compiler} {link_flags} {compiled_sources} -o {output} "
    "{lib_dirs} {lib_deps}";
constexpr const char *const kGccStaticLibLinkCommand =
    "{archiver} rcs {output} {compiled_sources}";
constexpr const char *const kGccDynamicLibLinkCommand =
    "{cpp_compiler} -shared {link_flags} {compiled_sources} -o {output}";

class GccConfig : ConfigInterface<GccConfig> {
public:
  static TargetConfig Executable() {
    return DefaultGccConfig(kGccExecutableExt, kGccGenericCompileCommand,
                            kGccExecutableLinkCommand);
  }
  static TargetConfig StaticLib() {
    return DefaultGccConfig(kGccStaticLibExt, kGccGenericCompileCommand,
                            kGccDynamicLibLinkCommand);
  }
  static TargetConfig DynamicLib() {
    return DefaultGccConfig(kGccDynamicLibExt, kGccGenericCompileCommand,
                            kGccDynamicLibLinkCommand);
  }

private:
  static TargetConfig DefaultGccConfig(const std::string &target_ext,
                                       const std::string &compile_command,
                                       const std::string &link_command) {
    TargetConfig config;
    config.target_ext = target_ext;
    config.obj_ext = kGccObjExt;
    config.pch_header_ext = kGccPchHeaderExt;
    config.pch_compile_ext = kGccPchCompileExt;
    std::string prefix_include_dir = kGccPrefixIncludeDir;
    std::string prefix_lib_dir = kGccPrefixLibDir;
    config.pch_command = kGccGenericPchCompileCommand;
    config.compile_command = compile_command;
    config.link_command = link_command;
    return config;
  }
};

inline void DefaultGccOptions(BaseTarget &target) {
  target.AddPchObjectFlag(
      fmt::format("-include {}",
                  fs::path(target.GetPchCompilePath()).replace_extension("")));
  target.AddPchObjectFlag("-H");
}

class ExecutableTarget_gcc : public BaseTarget {
public:
  ExecutableTarget_gcc(const std::string &name, const BaseToolchain &toolchain,
                       const Env &env,
                       const TargetConfig &config = GccConfig::Executable())
      : Target(name, BaseTarget::Type::Executable, toolchain, env, config) {
    DefaultGccOptions(*this);
  }
};

class StaticTarget_gcc : public BaseTarget {
public:
  StaticTarget_gcc(const std::string &name, const BaseToolchain &toolchain,
                   const Env &env,
                   const TargetConfig &config = GccConfig::StaticLib())
      : Target(name, BaseTarget::Type::StaticLibrary, toolchain, env, config) {
    DefaultGccOptions(*this);
  }
};

class DynamicTarget_gcc : public BaseTarget {
public:
  DynamicTarget_gcc(const std::string &name, const BaseToolchain &toolchain,
                    const Env &env,
                    const TargetConfig &config = GccConfig::DynamicLib())
      : Target(name, BaseTarget::Type::DynamicLibrary, toolchain, env, config) {
    AddCommonCompileFlag("-fpic");
    DefaultGccOptions(*this);
  }
};

} // namespace buildcc

#endif
