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
#include "buildexe/build_env_home.h"

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

void BuildExeArgs::Setup() {
  args_.AddToolchain("host", "Host Toolchain", host_toolchain_arg_);
  SetupBuildMode();
  SetupTargetInfo();
  SetupTargetInputs();
  SetupScriptMode();
  SetupLibs();
}

void BuildExeArgs::SetupBuildMode() {
  args_.Ref()
      .add_option("--mode", out_mode_, "Provide BuildExe run mode")
      ->transform(CLI::CheckedTransformer(kBuildExeModeMap, CLI::ignore_case))
      ->required();
}

// TODO, Add subcommand [build.info]
void BuildExeArgs::SetupTargetInfo() {
  constexpr const char *const kProjectInfo = "Project Info";
  auto &app = args_.Ref();

  auto *project_info_app = app.add_option_group(kProjectInfo);

  project_info_app
      ->add_option("--name", out_targetinfo_.name, "Provide Target name")
      ->required();

  project_info_app
      ->add_option("--type", out_targetinfo_.type, "Provide Target Type")
      ->transform(CLI::CheckedTransformer(kTargetTypeMap, CLI::ignore_case))
      ->required();

  project_info_app
      ->add_option("--relative_to_root", out_targetinfo_.relative_to_root,
                   "Provide Target relative to root")
      ->required();
}

// TODO, Add subcommand [build.inputs]
// TODO, Add group, group by sources, headers, inncludes on CLI
void BuildExeArgs::SetupTargetInputs() {
  constexpr const char *const kTargetInputs = "Target Inputs";
  auto &app = args_.Ref();

  auto *target_inputs_app = app.add_option_group(kTargetInputs);

  target_inputs_app->add_option("--srcs", out_targetinputs_.source_files,
                                "Provide source files");
  target_inputs_app->add_option("--includes", out_targetinputs_.include_dirs,
                                "Provide include dirs");

  target_inputs_app->add_option("--lib_dirs", out_targetinputs_.lib_dirs,
                                "Provide lib dirs");
  target_inputs_app->add_option("--external_libs",
                                out_targetinputs_.external_lib_deps,
                                "Provide external libs");

  target_inputs_app->add_option("--preprocessor_flags",
                                out_targetinputs_.preprocessor_flags,
                                "Provide Preprocessor flags");
  target_inputs_app->add_option("--common_compile_flags",
                                out_targetinputs_.common_compile_flags,
                                "Provide CommonCompile Flags");
  target_inputs_app->add_option("--asm_compile_flags",
                                out_targetinputs_.asm_compile_flags,
                                "Provide AsmCompile Flags");
  target_inputs_app->add_option("--c_compile_flags",
                                out_targetinputs_.c_compile_flags,
                                "Provide CCompile Flags");
  target_inputs_app->add_option("--cpp_compile_flags",
                                out_targetinputs_.cpp_compile_flags,
                                "Provide CppCompile Flags");
  target_inputs_app->add_option("--link_flags", out_targetinputs_.link_flags,
                                "Provide Link Flags");
}

void BuildExeArgs::SetupScriptMode() {
  auto *script_args = args_.Ref().add_subcommand("script");
  script_args->add_option("--configs", out_scriptinfo_.configs,
                          "Config files for script mode");
}

void BuildExeArgs::SetupLibs() {
  auto *libs_app = args_.Ref().add_subcommand("libs", "Libraries");
  std::error_code ec;
  fs::directory_iterator dir_iter =
      fs::directory_iterator(BuildccHome::GetBuildccLibsDir(), ec);
  env::assert_fatal(ec.value() == 0,
                    "Cannot iterate over {BUILDCC_HOME}/libs directory");

  for (const auto &dir : dir_iter) {
    if (!dir.is_directory()) {
      continue;
    }
    fs::path lib_path = dir.path();
    std::string lib_name = lib_path.filename().string();
    libs_app->add_option_function<std::vector<std::string>>(
        fmt::format("--{}", lib_name),
        [&lib_path, this](const std::vector<std::string> &paths) -> bool {
          for (const auto &p : paths) {
            fs::path absolute_file_path = lib_path / p;
            lib_build_files_.push_back(absolute_file_path);
          }
          return true;
        },
        fmt::format("{} library", lib_name));
  }
}

} // namespace buildcc
