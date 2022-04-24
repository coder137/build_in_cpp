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

#ifndef BUILDEXE_BUILD_ENV_SETUP_H_
#define BUILDEXE_BUILD_ENV_SETUP_H_

#include "buildcc.h"

#include "bootstrap/build_buildcc.h"

#include "buildexe/args_setup.h"

namespace buildcc {

class BuildEnvSetup {
public:
  static constexpr const char *const kBuildccPackageName = "BuildccPackage";
  static constexpr const char *const kUserTargetName = "UserTarget";

public:
  BuildEnvSetup(const BaseToolchain &toolchain,
                const BuildExeArgs &buildexe_args)
      : toolchain_(toolchain), buildexe_args_(buildexe_args) {}

  void Setup(const ArgToolchainState &state);

  void RunUserTarget(const ArgScriptInfo &arg_script_info);

  // Getters
  StaticTarget_generic &GetBuildcc() {
    return storage_.Ref<BuildBuildCC>(kBuildccPackageName).GetBuildcc();
  }
  Target_generic &GetUserTarget() {
    return storage_.Ref<Target_generic>(kUserTargetName);
  }

private:
  void ConstructUserTarget();
  void ConstructUserTargetWithBuildcc();

  void BuildccTargetSetup();
  void UserTargetSetup();
  void UserTargetCb();
  void UserTargetWithBuildccSetup();
  void UserTargetWithLibsSetup();
  void UserTargetBuild();
  void DepUserTargetOnBuildcc();

private:
  const BaseToolchain &toolchain_;
  const BuildExeArgs &buildexe_args_;

  ArgToolchainState state_;
  ScopedStorage storage_;
};

} // namespace buildcc

#endif
