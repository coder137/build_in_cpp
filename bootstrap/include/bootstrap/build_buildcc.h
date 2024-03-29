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

#ifndef BOOTSTRAP_BUILD_BUILDCC_H_
#define BOOTSTRAP_BUILD_BUILDCC_H_

#include "buildcc.h"

#include "build_cli11.h"
#include "build_fmtlib.h"
#include "build_nlohmann_json.h"
#include "build_spdlog.h"
#include "build_taskflow.h"
#include "build_tl_optional.h"
#include "build_tpl.h"

namespace buildcc {

void buildcc_cb(BaseTarget &target, const TargetInfo &nlohmann_json_ho,
                const TargetInfo &fmt_ho, const TargetInfo &spdlog_ho,
                const TargetInfo &cli11_ho, const TargetInfo &taskflow_ho,
                const TargetInfo &tl_optional_ho, const BaseTarget &tpl);

/**
 * @brief
 *
 */
class BuildBuildCC {
public:
  // TargetInfo / Header Only
  static constexpr const char *const kNlohmannJsonHoName = "nlohmann_json_ho";
  static constexpr const char *const kCli11HoName = "cli11_ho";
  static constexpr const char *const kFmtHoName = "fmtlib_ho";
  static constexpr const char *const kSpdlogHoName = "spdlog_ho";
  static constexpr const char *const kTaskflowHoName = "taskflow_ho";
  static constexpr const char *const kTlOptionalHoName = "tl_optional_ho";

  // Executable
  static constexpr const char *const kFlatcExeName = "flatc";

  // Libraries
  static constexpr const char *const kTplLibName = "libtpl";
  static constexpr const char *const kBuildccLibName = "libbuildcc";

public:
  BuildBuildCC(const BaseToolchain &toolchain, const TargetEnv &env)
      : toolchain_(toolchain), env_(env) {
    Initialize();
  }
  BuildBuildCC(const BuildBuildCC &) = delete;

  void Setup(const ArgToolchainState &state);

  // Getters
  StaticTarget_generic &GetTpl() {
    return storage_.Ref<StaticTarget_generic>(kTplLibName);
  }
  StaticTarget_generic &GetBuildcc() {
    return storage_.Ref<StaticTarget_generic>(kBuildccLibName);
  }

private:
  void Initialize();
  TargetInfo &GetNlohmannJsonHo() {
    return storage_.Ref<TargetInfo>(kNlohmannJsonHoName);
  }
  TargetInfo &GetCli11Ho() { return storage_.Ref<TargetInfo>(kCli11HoName); }
  TargetInfo &GetFmtHo() { return storage_.Ref<TargetInfo>(kFmtHoName); }
  TargetInfo &GetSpdlogHo() { return storage_.Ref<TargetInfo>(kSpdlogHoName); }
  TargetInfo &GetTaskflowHo() {
    return storage_.Ref<TargetInfo>(kTaskflowHoName);
  }
  TargetInfo &GetTlOptionalHo() {
    return storage_.Ref<TargetInfo>(kTlOptionalHoName);
  }

private:
  const BaseToolchain &toolchain_;
  TargetEnv env_;

  ScopedStorage storage_;
};

} // namespace buildcc

#endif
