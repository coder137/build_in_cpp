/*
 * Copyright 2021 Niket Naidu. All rights reserved.
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

void schema_gen_cb(BaseGenerator &generator, const BaseTarget &flatc_exe) {
  generator.AddInput("{gen_root_dir}/path.fbs", "path_fbs");
  generator.AddInput("{gen_root_dir}/generator.fbs", "generator_fbs");
  generator.AddInput("{gen_root_dir}/target.fbs", "target_fbs");

  generator.AddOutput("{gen_build_dir}/path_generated.h");
  generator.AddOutput("{gen_build_dir}/generator_generated.h");
  generator.AddOutput("{gen_build_dir}/target_generated.h");

  generator.AddDefaultArguments({
      {"flatc_compiler", fmt::format("{}", flatc_exe.GetTargetPath())},
  });
  //   generator.AddCommand("{flatc_compiler} --help");
  generator.AddCommand(
      "{flatc_compiler} -o {gen_build_dir} -I {gen_root_dir} --gen-object-api "
      "--cpp {path_fbs} {generator_fbs} {target_fbs}");

  generator.Build();
}

void buildcc_cb(BaseTarget &target, const BaseGenerator &schema_gen,
                const TargetInfo &flatbuffers_ho, const TargetInfo &fmt_ho,
                const TargetInfo &spdlog_ho, const TargetInfo &cli11_ho,
                const TargetInfo &taskflow_ho, const BaseTarget &tpl) {
  // NOTE, Build as single lib
  target.AddIncludeDir("", true);
  const std::string &schema_build_dir =
      schema_gen.GetValueByIdentifier("gen_build_dir");
  target.AddIncludeDirAbsolute(schema_build_dir, true);

  // ENV
  target.GlobSources("lib/env/src");
  target.AddIncludeDir("lib/env/include");
  target.GlobHeaders("lib/env/include/env");

  // TOOLCHAIN
  target.AddIncludeDir("lib/toolchain/include");
  target.GlobHeaders("lib/toolchain/include/toolchain");

  // TARGET
  target.GlobSources("lib/target/src/common");
  target.GlobSources("lib/target/src/generator");
  target.GlobSources("lib/target/src/api");
  target.GlobSources("lib/target/src/target");
  target.GlobSources("lib/target/src/target/friend");

  target.AddIncludeDir("lib/target/include");
  target.GlobHeaders("lib/target/include/target");
  target.GlobHeaders("lib/target/include/target/api");
  target.GlobHeaders("lib/target/include/target/base");
  target.GlobHeaders("lib/target/include/target/common");
  target.GlobHeaders("lib/target/include/target/friend");
  target.GlobHeaders("lib/target/include/target/interface");
  target.GlobHeaders("lib/target/include/target/private");

  // ARGS
  target.GlobSources("lib/args/src");
  target.AddIncludeDir("lib/args/include");
  target.GlobHeaders("lib/args/include/args");

  // Specialized Toolchains
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

  // FLATBUFFERS HO
  target.Insert(flatbuffers_ho, kInsertOptions);

  // FMT HO
  target.Insert(fmt_ho, kInsertOptions);

  // SPDLOG HO
  target.Insert(spdlog_ho, kInsertOptions);

  // CLI11 HO
  target.Insert(cli11_ho, kInsertOptions);

  // TASKFLOW HO
  target.Insert(taskflow_ho, kInsertOptions);

  // TPL LIB
  target.AddLibDep(tpl);
  target.Insert(tpl, kInsertOptions);

  if constexpr (env::is_win()) {
    // TODO, Clang
    switch (target.GetToolchain().GetId()) {
    case ToolchainId::Gcc:
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

  if constexpr (env::is_linux()) {
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

void BuildBuildCC::Setup(const ArgToolchainState &state) {
  auto &flatc_exe = storage_.Add<ExecutableTarget_generic>(
      kFlatcExeName, kFlatcExeName, toolchain_,
      TargetEnv(env_.GetTargetRootDir() / "third_party" / "flatbuffers",
                env_.GetTargetBuildDir()));

  reg_.CallbackIf(state, global_flags_cb, flatc_exe, toolchain_);
  reg_.Build(state, build_flatc_exe_cb, flatc_exe);

  // Schema
  auto &schema_gen = storage_.Add<BaseGenerator>(
      kSchemaGenName, kSchemaGenName,
      TargetEnv(env_.GetTargetRootDir() / "buildcc" / "schema",
                env_.GetTargetBuildDir() / toolchain_.GetName()));
  reg_.Build(schema_gen_cb, schema_gen, flatc_exe);
  reg_.Dep(schema_gen, flatc_exe);

  // Flatbuffers HO lib
  auto &flatbuffers_ho_lib = storage_.Add<TargetInfo>(
      kFlatbuffersHoName,
      TargetEnv(env_.GetTargetRootDir() / "third_party" / "flatbuffers",
                env_.GetTargetBuildDir()));
  reg_.CallbackIf(state, flatbuffers_ho_cb, flatbuffers_ho_lib);

  // CLI11 HO lib
  auto &cli11_ho_lib = storage_.Add<TargetInfo>(
      kCli11HoName, TargetEnv(env_.GetTargetRootDir() / "third_party" / "CLI11",
                              env_.GetTargetBuildDir()));
  reg_.CallbackIf(state, cli11_ho_cb, cli11_ho_lib);

  // fmt HO lib
  auto &fmt_ho_lib = storage_.Add<TargetInfo>(
      kFmtHoName, TargetEnv(env_.GetTargetRootDir() / "third_party" / "fmt",
                            env_.GetTargetBuildDir()));
  reg_.CallbackIf(state, fmt_ho_cb, fmt_ho_lib);

  // spdlog HO lib
  auto &spdlog_ho_lib = storage_.Add<TargetInfo>(
      kSpdlogHoName,
      TargetEnv(env_.GetTargetRootDir() / "third_party" / "spdlog",
                env_.GetTargetBuildDir()));
  reg_.CallbackIf(state, spdlog_ho_cb, spdlog_ho_lib);

  // taskflow HO lib
  auto &taskflow_ho_lib = storage_.Add<TargetInfo>(
      kTaskflowHoName,
      TargetEnv(env_.GetTargetRootDir() / "third_party" / "taskflow",
                env_.GetTargetBuildDir()));
  reg_.CallbackIf(state, taskflow_ho_cb, taskflow_ho_lib);

  // Tiny-process-library lib
  // TODO, Make this a generic selection between StaticTarget and
  // DynamicTarget
  auto &tpl_lib = storage_.Add<StaticTarget_generic>(
      kTplLibName, kTplLibName, toolchain_,
      TargetEnv(env_.GetTargetRootDir() / "third_party" /
                    "tiny-process-library",
                env_.GetTargetBuildDir()));
  reg_.CallbackIf(state, global_flags_cb, tpl_lib, toolchain_);
  TplConfig tpl_config;
  tpl_config.os_id = get_host_os();
  reg_.Build(state, tpl_cb, tpl_lib, tpl_config);

  // TODO, Make this a generic selection between StaticTarget and
  // DynamicTarget
  auto &buildcc_lib = storage_.Add<StaticTarget_generic>(
      kBuildccLibName, kBuildccLibName, toolchain_,
      TargetEnv(env_.GetTargetRootDir() / "buildcc", env_.GetTargetBuildDir()));
  reg_.CallbackIf(state, global_flags_cb, buildcc_lib, toolchain_);
  reg_.Build(state, buildcc_cb, buildcc_lib, schema_gen, flatbuffers_ho_lib,
             fmt_ho_lib, spdlog_ho_lib, cli11_ho_lib, taskflow_ho_lib, tpl_lib);
  reg_.Dep(buildcc_lib, schema_gen);
  reg_.Dep(buildcc_lib, tpl_lib);
}

} // namespace buildcc
