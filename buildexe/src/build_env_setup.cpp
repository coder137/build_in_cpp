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

#include "buildexe/build_env_setup.h"
#include "buildexe/build_env_home.h"

namespace buildcc {

constexpr const char *const kTag = "BuildExe";

void BuildEnvSetup::ConstructTarget() {
  if (buildexe_args_.GetBuildMode() == BuildExeMode::Script) {
    // buildcc and user target
    ConstructUserTargetWithBuildcc();
  } else {
    // user target
    ConstructUserTarget();
  }
  Reg::Run();
}

void BuildEnvSetup::RunUserTarget(const ArgScriptInfo &arg_script_info) {
  env::log_info(kTag, fmt::format("************** Running '{}' **************",
                                  buildexe_args_.GetTargetInfo().name));

  // Aggregate the different input build .toml files to
  // `--config .toml` files
  std::vector<std::string> configs;
  std::transform(arg_script_info.configs.begin(), arg_script_info.configs.end(),
                 std::back_inserter(configs),
                 [](const std::string &c) -> std::string {
                   return fmt::format("--config {}", c);
                 });
  std::string aggregated_configs = fmt::format("{}", fmt::join(configs, " "));

  // Construct and execute with user target on subprocess
  std::string command_str =
      fmt::format("{executable} {configs}",
                  fmt::arg("executable",
                           fmt::format("{}", GetUserTarget().GetTargetPath())),
                  fmt::arg("configs", aggregated_configs));
  env::Command::Execute(command_str);
}

// Private

void BuildEnvSetup::ConstructUserTarget() {
  UserTargetSetup();
  UserTargetCb();
  UserTargetBuild();
}

void BuildEnvSetup::ConstructUserTargetWithBuildcc() {
  BuildccTargetSetup();
  UserTargetSetup();
  UserTargetCb();
  UserTargetWithBuildccSetup();
  UserTargetWithLibsSetup();
  UserTargetBuild();
  DepUserTargetOnBuildcc();
}

void BuildEnvSetup::BuildccTargetSetup() {
  const fs::path &buildcc_base = BuildccHome::GetBuildccBaseDir();
  auto &buildcc_package = storage_.Add<BuildBuildCC>(
      kBuildccPackageName, toolchain_,
      TargetEnv(buildcc_base, buildcc_base / "_build_exe"));
  buildcc_package.Setup(state_);
}

void BuildEnvSetup::UserTargetSetup() {
  const ArgTargetInfo &arg_target_info = buildexe_args_.GetTargetInfo();
  storage_.Add<Target_generic>(kUserTargetName, arg_target_info.name,
                               arg_target_info.type, toolchain_,
                               TargetEnv(arg_target_info.relative_to_root));
}

/**
 * @brief Adds from Arg Target Inputs
 *
 * Source files
 * Include Dirs
 * Lib Dirs
 * External Lib Deps
 * Preprocessor flags
 * Common Compile flags
 * Asm Compile flags
 * C Compile flags
 * Cpp Compile flags
 * Link flags
 */
void BuildEnvSetup::UserTargetCb() {
  const ArgTargetInputs arg_target_inputs = buildexe_args_.GetTargetInputs();
  Target_generic &user_target = GetUserTarget();

  for (const auto &s : arg_target_inputs.source_files) {
    user_target.AddSource(s);
  }
  for (const auto &i : arg_target_inputs.include_dirs) {
    user_target.AddIncludeDir(i);
  }
  for (const auto &l : arg_target_inputs.lib_dirs) {
    user_target.AddLibDir(l);
  }
  for (const auto &el : arg_target_inputs.external_lib_deps) {
    user_target.AddLibDep(el);
  }
  for (const auto &flag : arg_target_inputs.preprocessor_flags) {
    user_target.AddPreprocessorFlag(flag);
  }
  for (const auto &flag : arg_target_inputs.common_compile_flags) {
    user_target.AddCommonCompileFlag(flag);
  }
  for (const auto &flag : arg_target_inputs.asm_compile_flags) {
    user_target.AddAsmCompileFlag(flag);
  }
  for (const auto &flag : arg_target_inputs.c_compile_flags) {
    user_target.AddCCompileFlag(flag);
  }
  for (const auto &flag : arg_target_inputs.cpp_compile_flags) {
    user_target.AddCppCompileFlag(flag);
  }
  for (const auto &flag : arg_target_inputs.link_flags) {
    user_target.AddLinkFlag(flag);
  }
}

void BuildEnvSetup::UserTargetWithBuildccSetup() {
  GetUserTarget().AddLibDep(GetBuildcc());
  GetUserTarget().Insert(GetBuildcc(), {
                                           SyncOption::PreprocessorFlags,
                                           SyncOption::CppCompileFlags,
                                           SyncOption::IncludeDirs,
                                           SyncOption::LinkFlags,
                                           SyncOption::HeaderFiles,
                                           SyncOption::IncludeDirs,
                                           SyncOption::LibDeps,
                                           SyncOption::ExternalLibDeps,
                                       });
  switch (GetUserTarget().GetToolchain().GetId()) {
  case ToolchainId::MinGW:
    GetUserTarget().AddLinkFlag("-Wl,--allow-multiple-definition");
    break;
  default:
    break;
  }
}

void BuildEnvSetup::UserTargetWithLibsSetup() {
  auto &user_target = GetUserTarget();

  // Generate buildexe_lib_dirs.h with the absolute path to library folders
  // Query the information through BuildExeLibDir::[lib_folder_name]
  {
    constexpr const char *const kConstexprLibNameFormat =
        "static constexpr const char *const {lib_name} = "
        "\"{absolute_lib_dir}\";";
    constexpr const char *const kLibDirsFormat = R"(// Generated by BuildCC
#pragma once

struct BuildExeLibDir {{
{lib_dirs}
}};
)";

    const auto &libs_info = buildexe_args_.GetLibsInfo();
    std::vector<std::string> lib_constants;
    for (const auto &linfo : libs_info) {
      std::string lib_constant = fmt::format(
          kConstexprLibNameFormat, fmt::arg("lib_name", linfo.lib_name),
          fmt::arg("absolute_lib_dir", linfo.absolute_lib_path));
      lib_constants.push_back(lib_constant);
    }
    fs::path lib_dirs_filename =
        user_target.GetTargetBuildDir() / "buildexe_lib_dirs.h";
    std::string data = fmt::format(
        kLibDirsFormat, fmt::arg("lib_dirs", fmt::join(lib_constants, "\r\n")));
    env::save_file(lib_dirs_filename.string().c_str(), data, false);

    user_target.AddIncludeDirAbsolute(user_target.GetTargetBuildDir(), true);
  }

  // Segregate valid lib files into sources and include dirs
  for (const auto &lib_build_file : buildexe_args_.GetLibBuildFiles()) {
    if (user_target.GetToolchain().GetConfig().IsValidSource(lib_build_file)) {
      user_target.AddSourceAbsolute(lib_build_file);
    }
    if (user_target.GetToolchain().GetConfig().IsValidHeader(lib_build_file)) {
      user_target.AddIncludeDirAbsolute(lib_build_file.parent_path(), false);
      user_target.AddHeaderAbsolute(lib_build_file);
    }
  }
}

void BuildEnvSetup::UserTargetBuild() {
  Reg::Toolchain(state_).Build([](BaseTarget &target) { target.Build(); },
                               GetUserTarget());
}

void BuildEnvSetup::DepUserTargetOnBuildcc() {
  Reg::Toolchain(state_).Dep(GetUserTarget(), GetBuildcc());
}

} // namespace buildcc
