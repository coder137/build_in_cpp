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
#include "build_flatbuffers.h"
#include "build_fmtlib.h"
#include "build_spdlog.h"
#include "build_taskflow.h"
#include "build_tpl.h"

namespace buildcc {

void schema_gen_cb(BaseGenerator &generator, const BaseTarget &flatc_exe);

void buildcc_cb(BaseTarget &target, const BaseGenerator &schema_gen,
                const TargetInfo &flatbuffers_ho, const TargetInfo &fmt_ho,
                const TargetInfo &spdlog_ho, const TargetInfo &cli11_ho,
                const TargetInfo &taskflow_ho, const BaseTarget &tpl);

/**
 * @brief
 *
 */
class BuildBuildCC {
public:
  // TargetInfo / Header Only
  static constexpr const char *const kFlatbuffersHoName = "flatbuffers_ho";
  static constexpr const char *const kCli11HoName = "cli11_ho";
  static constexpr const char *const kFmtHoName = "fmtlib_ho";
  static constexpr const char *const kSpdlogHoName = "spdlog_ho";
  static constexpr const char *const kTaskflowHoName = "taskflow_ho";

  // Executable
  static constexpr const char *const kFlatcExeName = "flatc";

  // Generator
  static constexpr const char *const kSchemaGenName = "schema_gen";

  // Libraries
  static constexpr const char *const kTplLibName = "libtpl";
  static constexpr const char *const kBuildccLibName = "libbuildcc";

public:
  BuildBuildCC(const BaseToolchain &toolchain, const TargetEnv &env)
      : toolchain_(toolchain), env_(env) {}
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
  const BaseToolchain &toolchain_;
  TargetEnv env_;

  PersistentStorage storage_;
};

} // namespace buildcc

#endif
