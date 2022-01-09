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

#include "buildexe/args_setup.h"
#include "buildexe/build_env_home.h"
#include "buildexe/build_env_setup.h"
#include "buildexe/toolchain_setup.h"

#include "bootstrap/build_buildcc.h"
#include "bootstrap/build_cli11.h"
#include "bootstrap/build_flatbuffers.h"
#include "bootstrap/build_fmtlib.h"
#include "bootstrap/build_spdlog.h"
#include "bootstrap/build_taskflow.h"
#include "bootstrap/build_tpl.h"

using namespace buildcc;

constexpr const char *const kTag = "BuildExe";

static void clean_cb();

// TODO, Update BuildExeArgs with internal functions
int main(int argc, char **argv) {
  //
  BuildccHome::Init();

  //
  BuildExeArgs buildexe_args;
  buildexe_args.Setup();
  buildexe_args.Parse(argc, argv);

  // TODO, Add Verification subcommand here for OS, Compiler etc!
  // os win, linux considerations
  // compiler gcc, msvc considerations
  // arch considerations

  // TODO, Add buildcc (git cloned)
  // TODO, Add libraries (git cloned)
  // TODO, Add extension (git cloned)

  Register reg(buildexe_args.GetArgs());
  reg.Clean(clean_cb);

  // Host Toolchain
  BaseToolchain toolchain =
      buildexe_args.GetHostToolchainArg().ConstructToolchain();
  find_toolchain_verify(toolchain);
  if (buildexe_args.GetBuildMode() == BuildExeMode::Script) {
    host_toolchain_verify(toolchain);
  }

  // Build Target
  BuildEnvSetup build_setup(reg, toolchain, buildexe_args);
  build_setup.ConstructTarget();

  // Run Target if script mode
  if (buildexe_args.GetBuildMode() == BuildExeMode::Script) {
    build_setup.RunUserTarget(buildexe_args.GetScriptInfo());
  }

  // - Clang Compile Commands
  plugin::ClangCompileCommands({&build_setup.GetUserTarget()}).Generate();

  return 0;
}

static void clean_cb() {
  env::log_info(kTag, fmt::format("Cleaning {}", env::get_project_build_dir()));
  fs::remove_all(env::get_project_build_dir());
}
