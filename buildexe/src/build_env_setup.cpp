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

namespace buildcc {

fs::path BuildccHome::buildcc_home_{""};
fs::path BuildccHome::buildcc_base_{""};
fs::path BuildccHome::buildcc_libs_{""};
fs::path BuildccHome::buildcc_extensions_{""};
bool BuildccHome::initialized_{false};

void BuildccHome::Init() {
  env::assert_fatal(!initialized_, "BuildccHome is already initialized");

  const char *buildcc_home = getenv("BUILDCC_HOME");
  env::assert_fatal(buildcc_home != nullptr,
                    "BUILDCC_HOME environment variable not defined");

  // NOTE, Verify BUILDCC_HOME
  buildcc_home_ = fs::path(buildcc_home);
  buildcc_base_ = buildcc_home_ / "buildcc";
  buildcc_libs_ = buildcc_home_ / "libs";
  buildcc_extensions_ = buildcc_home_ / "extensions";

  env::assert_fatal(fs::exists(buildcc_home_), "{BUILDCC_HOME} path not found");
  env::assert_fatal(fs::exists(buildcc_base_),
                    "{BUILDCC_HOME}/buildcc path not found");
  env::assert_fatal(fs::exists(buildcc_libs_),
                    "{BUILDCC_HOME}/libs path not found");
  env::assert_fatal(fs::exists(buildcc_extensions_),
                    "{BUILDCC_HOME}/extensions path not found");

  initialized_ = true;
}

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
  UserTargetBuild();
  DepUserTargetOnBuildcc();
}

void BuildEnvSetup::RunUserTarget(const ArgScriptInfo &arg_script_info) {
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

void BuildEnvSetup::DepUserTargetOnBuildcc() {
  reg_.Dep(GetUserTarget(), GetBuildcc());
}

void BuildEnvSetup::BuildccTargetSetup() {
  const fs::path &buildcc_base = BuildccHome::GetBuildccBaseDir();
  auto &buildcc_package = storage_.Add<BuildBuildCC>(
      kBuildccPackageName, reg_, toolchain_,
      TargetEnv(buildcc_base, buildcc_base / "_build_exe"));
  buildcc_package.Setup(state_);
}

void BuildEnvSetup::UserTargetSetup() {
  storage_.Add<Target_generic>(kUserTargetName, arg_target_info_.name,
                               arg_target_info_.type, toolchain_,
                               TargetEnv(arg_target_info_.relative_to_root));
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
  Target_generic &user_target = GetUserTarget();
  for (const auto &s : arg_target_inputs_.source_files) {
    user_target.AddSource(s);
  }
  for (const auto &i : arg_target_inputs_.include_dirs) {
    user_target.AddIncludeDir(i);
  }
  for (const auto &l : arg_target_inputs_.lib_dirs) {
    user_target.AddLibDir(l);
  }
  for (const auto &el : arg_target_inputs_.external_lib_deps) {
    user_target.AddLibDep(el);
  }
  for (const auto &flag : arg_target_inputs_.preprocessor_flags) {
    user_target.AddPreprocessorFlag(flag);
  }
  for (const auto &flag : arg_target_inputs_.common_compile_flags) {
    user_target.AddCommonCompileFlag(flag);
  }
  for (const auto &flag : arg_target_inputs_.asm_compile_flags) {
    user_target.AddAsmCompileFlag(flag);
  }
  for (const auto &flag : arg_target_inputs_.c_compile_flags) {
    user_target.AddCCompileFlag(flag);
  }
  for (const auto &flag : arg_target_inputs_.cpp_compile_flags) {
    user_target.AddCppCompileFlag(flag);
  }
  for (const auto &flag : arg_target_inputs_.link_flags) {
    user_target.AddLinkFlag(flag);
  }
}

void BuildEnvSetup::UserTargetBuild() {
  reg_.Build(
      state_, [](BaseTarget &target) { target.Build(); }, GetUserTarget());
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

} // namespace buildcc
