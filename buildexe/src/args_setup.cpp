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

#include "buildexe/args_setup.h"

namespace buildcc {

static const std::unordered_map<const char *, BuildExeMode> kBuildExeModeMap{
    {"immediate", BuildExeMode::Immediate},
    {"script", BuildExeMode::Script},
};

static const std::unordered_map<const char *, TargetType> kTargetTypeMap{
    {"executable", TargetType::Executable},
    {"staticLibrary", TargetType::StaticLibrary},
    {"dynamicLibrary", TargetType::DynamicLibrary},
};

void setup_arg_buildexe_mode(Args &args, BuildExeMode &out) {
  args.Ref()
      .add_option("--mode", out, "Provide BuildExe run mode")
      ->transform(CLI::CheckedTransformer(kBuildExeModeMap, CLI::ignore_case))
      ->required();
}

// TODO, Add subcommand [build.info]
void setup_arg_target_info(Args &args, ArgTargetInfo &out) {
  auto &app = args.Ref();

  app.add_option("--name", out.name, "Provide Target name")->required();

  app.add_option("--type", out.type, "Provide Target Type")
      ->transform(CLI::CheckedTransformer(kTargetTypeMap, CLI::ignore_case))
      ->required();

  app.add_option("--relative_to_root", out.relative_to_root,
                 "Provide Target relative to root")
      ->required();
}

// TODO, Add subcommand [build.inputs]
// TODO, Add group, group by sources, headers, inncludes on CLI
void setup_arg_target_inputs(Args &args, ArgTargetInputs &out) {
  auto &app = args.Ref();

  app.add_option("--srcs", out.source_files, "Provide source files");
  app.add_option("--includes", out.include_dirs, "Provide include dirs");

  app.add_option("--lib_dirs", out.lib_dirs, "Provide lib dirs");
  app.add_option("--external_libs", out.external_lib_deps,
                 "Provide external libs");

  app.add_option("--preprocessor_flags", out.preprocessor_flags,
                 "Provide Preprocessor flags");
  app.add_option("--common_compile_flags", out.common_compile_flags,
                 "Provide CommonCompile Flags");
  app.add_option("--asm_compile_flags", out.asm_compile_flags,
                 "Provide AsmCompile Flags");
  app.add_option("--c_compile_flags", out.c_compile_flags,
                 "Provide CCompile Flags");
  app.add_option("--cpp_compile_flags", out.cpp_compile_flags,
                 "Provide CppCompile Flags");
  app.add_option("--link_flags", out.link_flags, "Provide Link Flags");
}

void setup_arg_script_mode(Args &args, ArgScriptInfo &out) {
  auto *script_args = args.Ref().add_subcommand("script");
  script_args->add_option("--configs", out.configs,
                          "Config files for script mode");
}

} // namespace buildcc
