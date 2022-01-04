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
#include "target_gcc.h"

// MinGW
// ".exe", ".a", ".so" -> (x86_64-w64-mingw32)
namespace buildcc {

// Extensions
constexpr const char *const kMingwExecutableExt = ".exe";
constexpr const char *const kMingwDynamicLibExt = ".dll";

constexpr const char *const kMingwDynamicLibLinkCommand =
    "{cpp_compiler} -shared {link_flags} {compiled_sources} -o {output} "
    "-Wl,--out-implib,{output}.a";

class MingwConfig : ConfigInterface<MingwConfig> {
public:
  static TargetConfig Executable() {
    return DefaultMingwConfig(kMingwExecutableExt, kGccGenericCompileCommand,
                              kGccExecutableLinkCommand);
  }

  static TargetConfig DynamicLib() {
    return DefaultMingwConfig(kMingwDynamicLibExt, kGccGenericCompileCommand,
                              kMingwDynamicLibLinkCommand);
  }

private:
  static TargetConfig DefaultMingwConfig(const std::string &target_ext,
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

class ExecutableTarget_mingw : public ExecutableTarget_gcc {
public:
  ExecutableTarget_mingw(const std::string &name,
                         const BaseToolchain &toolchain, const TargetEnv &env,
                         const TargetConfig &config = MingwConfig::Executable())
      : ExecutableTarget_gcc(name, toolchain, env, config) {}
};

typedef StaticTarget_gcc StaticTarget_mingw;

class DynamicTarget_mingw : public DynamicTarget_gcc {
public:
  DynamicTarget_mingw(const std::string &name, const BaseToolchain &toolchain,
                      const TargetEnv &env,
                      const TargetConfig &config = MingwConfig::DynamicLib())
      : DynamicTarget_gcc(name, toolchain, env, config) {}
};

} // namespace buildcc

#endif
