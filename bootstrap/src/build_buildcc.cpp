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

#include "bootstrap/build_buildcc.h"

namespace buildcc {

void buildcc_cb(BaseTarget &target, const TargetInfo &nlohmann_json_ho,
                const TargetInfo &fmt_ho, const TargetInfo &spdlog_ho,
                const TargetInfo &cli11_ho, const TargetInfo &taskflow_ho,
                const TargetInfo &tl_optional_ho, const BaseTarget &tpl) {
  // NOTE, Build as single lib
  target.AddIncludeDir("", true);

  // ENV
  target.GlobSources("lib/env/src");
  target.AddIncludeDir("lib/env/include");
  target.GlobHeaders("lib/env/include/env");

  // SCHEMA
  target.GlobSources("schema/src");
  target.AddIncludeDir("schema/include");
  target.GlobHeaders("schema/include/schema");
  target.GlobHeaders("schema/include/schema/interface");

  // TOOLCHAIN
  target.GlobSources("lib/toolchain/src/api");
  target.GlobSources("lib/toolchain/src/common");
  target.GlobSources("lib/toolchain/src/toolchain");
  target.AddIncludeDir("lib/toolchain/include");
  target.GlobHeaders("lib/toolchain/include/toolchain");
  target.GlobHeaders("lib/toolchain/include/toolchain/api");
  target.GlobHeaders("lib/toolchain/include/toolchain/common");

  // TARGET
  target.GlobSources("lib/target/src/common");
  target.GlobSources("lib/target/src/custom_generator");
  target.GlobSources("lib/target/src/generator");
  target.GlobSources("lib/target/src/api");
  target.GlobSources("lib/target/src/target_info");
  target.GlobSources("lib/target/src/target");
  target.GlobSources("lib/target/src/target/friend");

  target.AddIncludeDir("lib/target/include");
  target.GlobHeaders("lib/target/include/target");
  target.GlobHeaders("lib/target/include/target/api");
  target.GlobHeaders("lib/target/include/target/common");
  target.GlobHeaders("lib/target/include/target/friend");
  target.GlobHeaders("lib/target/include/target/interface");

  // ARGS
  target.GlobSources("lib/args/src");
  target.AddIncludeDir("lib/args/include");
  target.GlobHeaders("lib/args/include/args");

  // Specialized Toolchains
  target.GlobSources("toolchains/src");
  target.AddIncludeDir("toolchains/include");
  target.GlobHeaders("toolchains/include/toolchains");

  // Specialized Targets
  target.AddIncludeDir("targets/include");
  target.GlobHeaders("targets/include/targets");

  // Plugins
  target.GlobSources("plugins/src");
  target.AddIncludeDir("plugins/include");
  target.GlobHeaders("plugins/include/plugins");

  // Third Party libraries

  const std::initializer_list<SyncOption> kInsertOptions{
      SyncOption::IncludeDirs,
      SyncOption::HeaderFiles,
  };

  // NLOHMANN JSON HO
  target.Insert(nlohmann_json_ho, kInsertOptions);

  // FMT HO
  target.Insert(fmt_ho, kInsertOptions);

  // SPDLOG HO
  target.Insert(spdlog_ho, kInsertOptions);

  // CLI11 HO
  target.Insert(cli11_ho, kInsertOptions);

  // TASKFLOW HO
  target.Insert(taskflow_ho, kInsertOptions);

  // TL OPTIONAL HO
  target.Insert(tl_optional_ho, kInsertOptions);

  // TPL LIB
  target.AddLibDep(tpl);
  target.Insert(tpl, kInsertOptions);

  if constexpr (env::is_win()) {
    // TODO, Clang
    switch (target.GetToolchain().GetId()) {
    case ToolchainId::MinGW: {
      target.AddPreprocessorFlag("-DFMT_HEADER_ONLY=1");
      target.AddPreprocessorFlag("-DSPDLOG_FMT_EXTERNAL");
      // For MINGW
      target.AddLinkFlag("-Wl,--allow-multiple-definition");
    } break;
    case ToolchainId::Msvc: {
      target.AddPreprocessorFlag("/DFMT_HEADER_ONLY=1");
      target.AddPreprocessorFlag("/DSPDLOG_FMT_EXTERNAL");
    } break;
    default:
      break;
    }
  }

  if constexpr (env::is_linux() || env::is_unix() || env::is_clang()) {
    // TODO, Clang
    switch (target.GetToolchain().GetId()) {
    case ToolchainId::Gcc: {
      target.AddPreprocessorFlag("-DFMT_HEADER_ONLY=1");
      target.AddPreprocessorFlag("-DSPDLOG_FMT_EXTERNAL");
      target.AddLibDep("-lpthread");
    } break;
    default:
      break;
    }
  }

  // TODO, Other OS's

  target.Build();
}

// TODO, Shift this inside BuildBuildcc class if required
// TODO, Add this to options
static void global_flags_cb(TargetInfo &global_info,
                            const BaseToolchain &toolchain) {
  // TODO, Clang
  switch (toolchain.GetId()) {
  case ToolchainId::Gcc:
  case ToolchainId::MinGW:
    global_info.AddCppCompileFlag("-std=c++17");
    global_info.AddCppCompileFlag("-Os");
    global_info.AddCppCompileFlag("-Wall");
    global_info.AddCppCompileFlag("-Wextra");
    global_info.AddCppCompileFlag("-Werror");
    break;
  case ToolchainId::Msvc:
    global_info.AddPreprocessorFlag("/D_CRT_SECURE_NO_WARNINGS");
    global_info.AddCppCompileFlag("/std:c++17");
    global_info.AddCppCompileFlag("/Ot");
    global_info.AddCppCompileFlag("/W4");
    global_info.AddCppCompileFlag("/WX");
  default:
    break;
  }
}

void BuildBuildCC::Initialize() {
  // Nlohmann json HO lib
  (void)storage_.Add<TargetInfo>(
      kNlohmannJsonHoName, toolchain_,
      TargetEnv(env_.GetTargetRootDir() / "third_party" / "json",
                env_.GetTargetBuildDir()));

  // CLI11 HO lib
  (void)storage_.Add<TargetInfo>(
      kCli11HoName, toolchain_,
      TargetEnv(env_.GetTargetRootDir() / "third_party" / "CLI11",
                env_.GetTargetBuildDir()));

  // fmt HO lib
  (void)storage_.Add<TargetInfo>(
      kFmtHoName, toolchain_,
      TargetEnv(env_.GetTargetRootDir() / "third_party" / "fmt",
                env_.GetTargetBuildDir()));

  // spdlog HO lib
  (void)storage_.Add<TargetInfo>(
      kSpdlogHoName, toolchain_,
      TargetEnv(env_.GetTargetRootDir() / "third_party" / "spdlog",
                env_.GetTargetBuildDir()));

  // taskflow HO lib
  (void)storage_.Add<TargetInfo>(
      kTaskflowHoName, toolchain_,
      TargetEnv(env_.GetTargetRootDir() / "third_party" / "taskflow",
                env_.GetTargetBuildDir()));

  // tl optional HO lib
  (void)storage_.Add<TargetInfo>(
      kTlOptionalHoName, toolchain_,
      TargetEnv(env_.GetTargetRootDir() / "third_party" / "tl_optional",
                env_.GetTargetBuildDir()));

  // Tiny-process-library lib
  // TODO, Make this a generic selection between StaticTarget and
  // DynamicTarget
  (void)storage_.Add<StaticTarget_generic>(
      kTplLibName, kTplLibName, toolchain_,
      TargetEnv(env_.GetTargetRootDir() / "third_party" /
                    "tiny-process-library",
                env_.GetTargetBuildDir()));

  // BuildCC lib
  // TODO, Make this a generic selection between StaticTarget and
  // DynamicTarget
  (void)storage_.Add<StaticTarget_generic>(
      kBuildccLibName, kBuildccLibName, toolchain_,
      TargetEnv(env_.GetTargetRootDir() / "buildcc", env_.GetTargetBuildDir()));
}

void BuildBuildCC::Setup(const ArgToolchainState &state) {
  auto &nlohmann_json_ho_lib = GetNlohmannJsonHo();
  auto &cli11_ho_lib = GetCli11Ho();
  auto &fmt_ho_lib = GetFmtHo();
  auto &spdlog_ho_lib = GetSpdlogHo();
  auto &taskflow_ho_lib = GetTaskflowHo();
  auto &tl_optional_ho_lib = GetTlOptionalHo();
  auto &tpl_lib = GetTpl();
  auto &buildcc_lib = GetBuildcc();
  Reg::Toolchain(state)
      .Func(nlohmann_json_ho_cb, nlohmann_json_ho_lib)
      .Func(cli11_ho_cb, cli11_ho_lib)
      .Func(fmt_ho_cb, fmt_ho_lib)
      .Func(spdlog_ho_cb, spdlog_ho_lib)
      .Func(taskflow_ho_cb, taskflow_ho_lib)
      .Func(tl_optional_ho_cb, tl_optional_ho_lib)
      .Func(global_flags_cb, tpl_lib, toolchain_)
      .Build(tpl_cb, tpl_lib)
      .Func(global_flags_cb, buildcc_lib, toolchain_)
      .Build(buildcc_cb, buildcc_lib, nlohmann_json_ho_lib, fmt_ho_lib,
             spdlog_ho_lib, cli11_ho_lib, taskflow_ho_lib, tl_optional_ho_lib,
             tpl_lib)
      .Dep(buildcc_lib, tpl_lib);
}

} // namespace buildcc
