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

#include "buildcc.h"

#include "bootstrap/buildcc_cli11.h"
#include "bootstrap/buildcc_flatbuffers.h"
#include "bootstrap/buildcc_fmtlib.h"
#include "bootstrap/buildcc_spdlog.h"
#include "bootstrap/buildcc_taskflow.h"
#include "bootstrap/buildcc_tpl.h"

using namespace buildcc;

static void clean_cb();

static void global_flags_cb(TargetInfo &global_info,
                            const BaseToolchain &toolchain);

static void schema_gen_cb(base::Generator &generator,
                          const BaseTarget &flatc_exe);

static void buildcc_cb(BaseTarget &target, const base::Generator &schema_gen,
                       const TargetInfo &flatbuffers_ho,
                       const TargetInfo &fmt_ho, const TargetInfo &spdlog_ho,
                       const TargetInfo &cli11_ho,
                       const TargetInfo &taskflow_ho, const BaseTarget &tpl);

static void hybrid_simple_example_cb(BaseTarget &target,
                                     const BaseTarget &libbuildcc);

int main(int argc, char **argv) {
  Args args;
  Args::ToolchainArg arg_toolchain;
  args.AddToolchain("custom", "Host Toolchain", arg_toolchain);
  args.Parse(argc, argv);

  Register reg(args);
  reg.Clean(clean_cb);

  BaseToolchain toolchain = arg_toolchain.ConstructToolchain();

  // Flatc Executable
  ExecutableTarget_generic flatc_exe("flatc", toolchain,
                                     "third_party/flatbuffers");
  reg.Build(arg_toolchain.state, build_flatc_exe_cb, flatc_exe);

  // Schema
  base::Generator schema_gen("schema_gen", "buildcc/schema");
  reg.Build(schema_gen_cb, schema_gen, flatc_exe);
  reg.Dep(schema_gen, flatc_exe);

  // Flatbuffers HO lib
  TargetInfo flatbuffers_ho_lib("third_party/flatbuffers");
  reg.Callback(flatbuffers_ho_cb, flatbuffers_ho_lib);

  // CLI11 HO lib
  TargetInfo cli11_ho_lib("third_party/CLI11");
  reg.Callback(cli11_ho_cb, cli11_ho_lib);

  // fmt HO lib
  TargetInfo fmt_ho_lib("third_party/fmt");
  reg.Callback(fmt_ho_cb, fmt_ho_lib);

  // spdlog HO lib
  TargetInfo spdlog_ho_lib("third_party/spdlog");
  reg.Callback(spdlog_ho_cb, spdlog_ho_lib);

  // taskflow HO lib
  TargetInfo taskflow_ho_lib("third_party/taskflow");
  reg.Callback(taskflow_ho_cb, taskflow_ho_lib);

  // Tiny-process-library lib
  // TODO, Make this a generic selection between StaticTarget and DynamicTarget
  StaticTarget_generic tpl_lib("libtpl", toolchain,
                               "third_party/tiny-process-library");
  reg.CallbackIf(arg_toolchain.state, global_flags_cb, tpl_lib, toolchain);
  reg.Build(arg_toolchain.state, tpl_cb, tpl_lib);

  // TODO, Make this a generic selection between StaticTarget and DynamicTarget
  StaticTarget_generic buildcc_lib("libbuildcc", toolchain, "buildcc");
  reg.CallbackIf(arg_toolchain.state, global_flags_cb, buildcc_lib, toolchain);
  reg.Build(arg_toolchain.state, buildcc_cb, buildcc_lib, schema_gen,
            flatbuffers_ho_lib, fmt_ho_lib, spdlog_ho_lib, cli11_ho_lib,
            taskflow_ho_lib, tpl_lib);
  reg.Dep(buildcc_lib, schema_gen);
  reg.Dep(buildcc_lib, tpl_lib);

  ExecutableTarget_generic buildcc_hybrid_simple_example(
      "buildcc_hybrid_simple_example", toolchain, "example/hybrid/simple");
  reg.Build(arg_toolchain.state, hybrid_simple_example_cb,
            buildcc_hybrid_simple_example, buildcc_lib);
  reg.Dep(buildcc_hybrid_simple_example, buildcc_lib);

  reg.RunBuild();

  reg.RunTest();

  // - Clang Compile Commands
  plugin::ClangCompileCommands({&buildcc_lib}).Generate();

  // - Plugin Graph
  std::string output = reg.GetTaskflow().dump();
  const bool saved = env::save_file("graph.dot", output, false);
  env::assert_fatal(saved, "Could not save graph.dot file");

  return 0;
}

static void clean_cb() {}

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
    break;
  case ToolchainId::Msvc:
    global_info.AddCppCompileFlag("/std:c++17");
    global_info.AddCppCompileFlag("/Ot");
  default:
    break;
  }
}

static void schema_gen_cb(base::Generator &generator,
                          const BaseTarget &flatc_exe) {
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

static void buildcc_cb(BaseTarget &target, const base::Generator &schema_gen,
                       const TargetInfo &flatbuffers_ho,
                       const TargetInfo &fmt_ho, const TargetInfo &spdlog_ho,
                       const TargetInfo &cli11_ho,
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

  // COMMAND
  target.GlobSources("lib/command/src");
  target.AddIncludeDir("lib/command/include");
  target.GlobHeaders("lib/command/include/command");

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

static void hybrid_simple_example_cb(BaseTarget &target,
                                     const BaseTarget &libbuildcc) {
  target.Insert(libbuildcc, {
                                SyncOption::PreprocessorFlags,
                                SyncOption::CppCompileFlags,
                                SyncOption::LinkFlags,
                                SyncOption::HeaderFiles,
                                SyncOption::IncludeDirs,
                                SyncOption::LibDeps,
                                SyncOption::ExternalLibDeps,
                            });
  target.AddSource("build.cpp");
  target.AddLibDep(libbuildcc);
  target.Build();
}
