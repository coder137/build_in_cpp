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

static fs::path get_env_buildcc_home() {
  const char *buildcc_home = getenv("BUILDCC_HOME");
  env::assert_fatal(buildcc_home != nullptr,
                    "BUILDCC_HOME environment variable not defined");

  // NOTE, Verify BUILDCC_HOME
  // auto &buildcc_path = storage.Add<fs::path>("buildcc_path", buildcc_home);
  fs::path buildcc_home_path{buildcc_home};
  env::assert_fatal(fs::exists(buildcc_home_path),
                    "{BUILDCC_HOME} path not found");
  env::assert_fatal(fs::exists(buildcc_home_path / "buildcc"),
                    "{BUILDCC_HOME}/buildcc path not found");
  env::assert_fatal(fs::exists(buildcc_home_path / "libs"),
                    "{BUILDCC_HOME}/libs path not found");
  env::assert_fatal(fs::exists(buildcc_home_path / "extensions"),
                    "{BUILDCC_HOME}/extensions path not found");

  return buildcc_home_path;
}

void BuildEnvSetup::UserTarget() {
  UserTargetSetup();
  UserTargetCb();
  UserTargetBuild();
}

void BuildEnvSetup::UserTargetWithBuildcc() {
  BuildccTargetSetup();
  UserTargetSetup();
  UserTargetCb();
  UserTargetWithBuildccSetup();
  UserTargetBuild();
  DepUserTargetOnBuildcc();
}

// Private

void BuildEnvSetup::DepUserTargetOnBuildcc() {
  reg_.Dep(GetUserTarget(), GetBuildcc());
}

void BuildEnvSetup::BuildccTargetSetup() {
  fs::path buildcc_home = get_env_buildcc_home();
  auto &buildcc_package = storage_.Add<BuildBuildCC>(
      kBuildccPackageName, reg_, toolchain_,
      TargetEnv(buildcc_home / "buildcc",
                buildcc_home / "buildcc" / "_build_exe"));
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
