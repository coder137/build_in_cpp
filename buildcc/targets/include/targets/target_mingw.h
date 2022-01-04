/*
 * Copyright 2021-2022 Niket Naidu. All rights reserved.
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

#ifndef TARGETS_TARGET_MINGW_H_
#define TARGETS_TARGET_MINGW_H_

#include "target/target.h"

#include "target_config_interface.h"

// Windows
// ".exe", ".a", ".a" && ".dll" -> MINGW frontend (x86_64-w64-mingw32)
namespace buildcc {

// Extensions
constexpr const char *const kMingwExecutableExt = ".exe";
constexpr const char *const kMingwStaticLibExt = ".a";
// Why is `kMingwDynamicLibExt != .so` but `.a` instead?
// See `kMingwDynamicLibLinkCommand`
// IMPLIB .a stubs are what is linked during link time
// OUT .dll needs to be present in the executable folder during runtime
constexpr const char *const kMingwDynamicLibExt = ".a";

constexpr const char *const kMingwObjExt = ".o";
constexpr const char *const kMingwPchHeaderExt = ".h";
constexpr const char *const kMingwPchCompileExt = ".gch";

constexpr const char *const kMingwPrefixIncludeDir = "-I";
constexpr const char *const kMingwPrefixLibDir = "-L";

constexpr const char *const kMingwGenericPchCompileCommand =
    "{compiler} {preprocessor_flags} {include_dirs} {common_compile_flags} "
    "{pch_compile_flags} {compile_flags} -o {output} -c {input}";
constexpr const char *const kMingwGenericCompileCommand =
    "{compiler} {preprocessor_flags} {include_dirs} {common_compile_flags} "
    "{pch_object_flags} {compile_flags} -o {output} -c {input}";
constexpr const char *const kMingwExecutableLinkCommand =
    "{cpp_compiler} {link_flags} {compiled_sources} -o {output} "
    "{lib_dirs} {lib_deps}";
constexpr const char *const kMingwStaticLibLinkCommand =
    "{archiver} rcs {output} {compiled_sources}";
constexpr const char *const kMingwDynamicLibLinkCommand =
    "{cpp_compiler} -shared {link_flags} {compiled_sources} -o {output}";

class MingwConfig : ConfigInterface<MingwConfig> {
public:
  static TargetConfig Executable() {
    return DefaultMingwConfig(kMingwExecutableExt, kMingwGenericCompileCommand,
                              kMingwExecutableLinkCommand);
  }
  static TargetConfig StaticLib() {
    return DefaultMingwConfig(kMingwStaticLibExt, kMingwGenericCompileCommand,
                              kMingwStaticLibLinkCommand);
  }
  static TargetConfig DynamicLib() {
    return DefaultMingwConfig(kMingwDynamicLibExt, kMingwGenericCompileCommand,
                              kMingwDynamicLibLinkCommand);
  }

private:
  static TargetConfig DefaultMingwConfig(const std::string &target_ext,
                                         const std::string &compile_command,
                                         const std::string &link_command) {
    TargetConfig config;
    config.target_ext = target_ext;
    config.obj_ext = kMingwObjExt;
    config.pch_header_ext = kMingwPchHeaderExt;
    config.pch_compile_ext = kMingwPchCompileExt;
    std::string prefix_include_dir = kMingwPrefixIncludeDir;
    std::string prefix_lib_dir = kMingwPrefixLibDir;
    config.pch_command = kMingwGenericPchCompileCommand;
    config.compile_command = compile_command;
    config.link_command = link_command;
    return config;
  }
};

inline void DefaultMingwOptions(BaseTarget &target) {
  target.AddPchObjectFlag(
      fmt::format("-include {}",
                  fs::path(target.GetPchCompilePath()).replace_extension("")));
  target.AddPchObjectFlag("-H");
}

class ExecutableTarget_mingw : public BaseTarget {
public:
  ExecutableTarget_mingw(const std::string &name,
                         const BaseToolchain &toolchain, const TargetEnv &env,
                         const TargetConfig &config = MingwConfig::Executable())
      : Target(name, TargetType::Executable, toolchain, env, config) {
    DefaultMingwOptions(*this);
  }
};

} // namespace buildcc

#endif
