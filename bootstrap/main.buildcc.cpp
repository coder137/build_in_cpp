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

static void hybrid_simple_example_cb(BaseTarget &target,
                                     const BaseTarget &libbuildcc);

int main(int argc, char **argv) {
  Args args;
  ArgToolchain custom_toolchain_arg;
  args.AddToolchain("host", "Host Toolchain", custom_toolchain_arg);
  args.Parse(argc, argv);

  Register reg(args);
  reg.Clean(clean_cb);

  BaseToolchain toolchain = custom_toolchain_arg.ConstructToolchain();
  // TODO, Add toolchain specific flags here
  toolchain.Lock();

  BuildBuildCC buildcc(
      reg, toolchain,
      TargetEnv(env::get_project_root_dir(), env::get_project_build_dir()));
  buildcc.Setup(custom_toolchain_arg.state);

  const auto &buildcc_lib = buildcc.GetBuildcc();
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

static void hybrid_simple_example_cb(BaseTarget &target,
                                     const BaseTarget &libbuildcc) {
  target.AddLibDep(libbuildcc);
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
  target.Build();
}
