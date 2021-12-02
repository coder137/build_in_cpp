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

#include "bootstrap/build_cli11.h"
#include "bootstrap/build_flatbuffers.h"
#include "bootstrap/build_fmtlib.h"
#include "bootstrap/build_spdlog.h"
#include "bootstrap/build_taskflow.h"
#include "bootstrap/build_tpl.h"

#include "bootstrap/build_buildcc.h"

using namespace buildcc;

static void clean_cb();
static void global_flags_cb(TargetInfo &global_info,
                            const BaseToolchain &toolchain);
static void hybrid_simple_example_cb(BaseTarget &target,
                                     const BaseTarget &libbuildcc);

void setup_buildcc_cb(PersistentStorage &storage, Register &reg,
                      const Args::ToolchainArg &custom_toolchain_arg);

int main(int argc, char **argv) {
  Args args;
  Args::ToolchainArg custom_toolchain_arg;
  args.AddToolchain("custom", "Host Toolchain", custom_toolchain_arg);
  args.Parse(argc, argv);

  Register reg(args);
  reg.Clean(clean_cb);

  BaseToolchain toolchain = custom_toolchain_arg.ConstructToolchain();

  PersistentStorage storage;
  setup_buildcc_cb(storage, reg, custom_toolchain_arg);

  const StaticTarget_generic &buildcc_lib =
      storage.ConstRef<StaticTarget_generic>("libbuildcc");

  ExecutableTarget_generic buildcc_hybrid_simple_example(
      "buildcc_hybrid_simple_example", toolchain, "example/hybrid/simple");
  reg.Build(custom_toolchain_arg.state, hybrid_simple_example_cb,
            buildcc_hybrid_simple_example, buildcc_lib);
  reg.Dep(buildcc_hybrid_simple_example, buildcc_lib);

  // Runners
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

void setup_buildcc_cb(PersistentStorage &storage, Register &reg,
                      const Args::ToolchainArg &custom_toolchain_arg) {

  BaseToolchain toolchain = custom_toolchain_arg.ConstructToolchain();

  // Flatc Executable
  ExecutableTarget_generic &flatc_exe = storage.Add<ExecutableTarget_generic>(
      "flatc", "flatc", toolchain, "third_party/flatbuffers");
  reg.CallbackIf(custom_toolchain_arg.state, global_flags_cb, flatc_exe,
                 toolchain);
  reg.Build(custom_toolchain_arg.state, build_flatc_exe_cb, flatc_exe);

  // Schema
  BaseGenerator &schema_gen =
      storage.Add<BaseGenerator>("schema_gen", "schema_gen", "buildcc/schema");
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
  StaticTarget_generic &tpl_lib = storage.Add<StaticTarget_generic>(
      "libtpl", "libtpl", toolchain, "third_party/tiny-process-library");
  reg.CallbackIf(custom_toolchain_arg.state, global_flags_cb, tpl_lib,
                 toolchain);
  reg.Build(custom_toolchain_arg.state, tpl_cb, tpl_lib);

  // TODO, Make this a generic selection between StaticTarget and DynamicTarget
  StaticTarget_generic &buildcc_lib = storage.Add<StaticTarget_generic>(
      "libbuildcc", "libbuildcc", toolchain, "buildcc");
  reg.CallbackIf(custom_toolchain_arg.state, global_flags_cb, buildcc_lib,
                 toolchain);
  reg.Build(custom_toolchain_arg.state, buildcc_cb, buildcc_lib, schema_gen,
            flatbuffers_ho_lib, fmt_ho_lib, spdlog_ho_lib, cli11_ho_lib,
            taskflow_ho_lib, tpl_lib);
  reg.Dep(buildcc_lib, schema_gen);
  reg.Dep(buildcc_lib, tpl_lib);
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
