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

constexpr const char *const kTag = "BuildExe";

static const std::unordered_map<const char *, BuildExeMode> kBuildExeModeMap{
    {"immediate", BuildExeMode::Immediate},
    {"script", BuildExeMode::Script},
};

static const std::unordered_map<const char *, TargetType> kTargetTypeMap{
    {"executable", TargetType::Executable},
    {"staticLibrary", TargetType::StaticLibrary},
    {"dynamicLibrary", TargetType::DynamicLibrary},
};

void BuildExeArgs::Setup(int argc, char **argv) {
  Args::Init()
      .AddToolchain("host", "Host Toolchain", host_toolchain_arg_)
      .AddCustomData(out_targetinfo_)
      .AddCustomData(out_targetinputs_)
      .AddCustomData(out_scriptinfo_)
      .AddCustomData(out_libsinfo_)
      .AddCustomCallback([&](CLI::App &app) { SetupBuildMode(app); })
      .Parse(argc, argv);
}

void BuildExeArgs::SetupBuildMode(CLI::App &app) {
  app.add_option("--mode", out_mode_, "Provide BuildExe run mode")
      ->transform(CLI::CheckedTransformer(kBuildExeModeMap, CLI::ignore_case))
      ->required();
}

void ArgTargetInfo::Add(CLI::App &app) {
  constexpr const char *const kProjectInfo = "Project Info";
  auto *project_info_app = app.add_option_group(kProjectInfo);

  project_info_app->add_option("--name", name, "Provide Target name")
      ->required();

  project_info_app->add_option("--type", type, "Provide Target Type")
      ->transform(CLI::CheckedTransformer(kTargetTypeMap, CLI::ignore_case))
      ->required();

  project_info_app
      ->add_option("--relative_to_root", relative_to_root,
                   "Provide Target relative to root")
      ->required();
}

void ArgTargetInputs::Add(CLI::App &app) {
  constexpr const char *const kTargetInputs = "Target Inputs";
  auto *target_inputs_app = app.add_option_group(kTargetInputs);

  target_inputs_app->add_option("--srcs", source_files, "Provide source files");
  target_inputs_app->add_option("--includes", include_dirs,
                                "Provide include dirs");

  target_inputs_app->add_option("--lib_dirs", lib_dirs, "Provide lib dirs");
  target_inputs_app->add_option("--external_libs", external_lib_deps,
                                "Provide external libs");

  target_inputs_app->add_option("--preprocessor_flags", preprocessor_flags,
                                "Provide Preprocessor flags");
  target_inputs_app->add_option("--common_compile_flags", common_compile_flags,
                                "Provide CommonCompile Flags");
  target_inputs_app->add_option("--asm_compile_flags", asm_compile_flags,
                                "Provide AsmCompile Flags");
  target_inputs_app->add_option("--c_compile_flags", c_compile_flags,
                                "Provide CCompile Flags");
  target_inputs_app->add_option("--cpp_compile_flags", cpp_compile_flags,
                                "Provide CppCompile Flags");
  target_inputs_app->add_option("--link_flags", link_flags,
                                "Provide Link Flags");
};

void ArgScriptInfo::Add(CLI::App &app) {
  auto *script_args = app.add_subcommand("script");
  script_args->add_option("--configs", configs, "Config files for script mode");
}

void ArgLibsInfo::Add(CLI::App &app) {
  auto *libs_app = app.add_subcommand("libs", "Libraries");
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

    LibInfo lib_info;
    lib_info.lib_name = lib_name;
    lib_info.absolute_lib_path = fmt::format("{}", lib_path);
    libs_info.push_back(lib_info);

    auto add_lib_files_cb_func = [lib_path,
                                  this](const std::vector<std::string> &paths) {
      for (const auto &p : paths) {
        fs::path absolute_file_path = (lib_path / p).make_preferred();
        lib_build_files.push_back(absolute_file_path);
      }
    };

    libs_app->add_option_function<std::vector<std::string>>(
        fmt::format("--{}", lib_name), add_lib_files_cb_func,
        fmt::format("{} library", lib_name));
  }
}

} // namespace buildcc
