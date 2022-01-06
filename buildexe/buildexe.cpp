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

// TODO, Add BuildExeMode::Script usage
int main(int argc, char **argv) {
  Args args;

  ArgToolchain host_toolchain_arg;
  args.AddToolchain("host", "Host Toolchain", host_toolchain_arg);

  BuildExeMode out_mode;
  setup_arg_buildexe_mode(args, out_mode);

  ArgTargetInfo out_targetinfo;
  setup_arg_target_info(args, out_targetinfo);

  ArgTargetInputs out_targetinputs;
  setup_arg_target_inputs(args, out_targetinputs);

  ArgScriptInfo out_scriptinfo;
  setup_arg_script_mode(args, out_scriptinfo);

  args.Parse(argc, argv);

  // TODO, Add Verification subcommand here for OS, Compiler etc!
  // os win, linux considerations
  // compiler gcc, msvc considerations
  // arch considerations

  // TODO, Add buildcc (git cloned)
  // TODO, Add libraries (git cloned)
  // TODO, Add extension (git cloned)

  Register reg(args);
  reg.Clean(clean_cb);

  // Build
  BaseToolchain toolchain = host_toolchain_arg.ConstructToolchain();
  find_toolchain_verify(toolchain);
  if (out_mode == BuildExeMode::Script) {
    host_toolchain_verify(toolchain);
  }

  BuildEnvSetup build_setup(reg, toolchain, out_targetinfo, out_targetinputs);
  if (out_mode == BuildExeMode::Script) {
    build_setup.UserTargetWithBuildcc();
  } else {
    build_setup.UserTarget();
  }

  // Runners
  reg.RunBuild();

  env::log_info(kTag, fmt::format("************** Running '{}' **************",
                                  out_targetinfo.name));

  // Run
  if (out_mode == BuildExeMode::Script) {
    std::vector<std::string> configs;
    for (const auto &c : out_scriptinfo.configs) {
      std::string config = fmt::format("--config {}", c);
      configs.push_back(config);
    }
    std::string aggregated_configs = fmt::format("{}", fmt::join(configs, " "));

    env::Command command;
    std::string command_str = command.Construct(
        "{executable} {configs}",
        {
            {"executable",
             fmt::format("{}", build_setup.GetUserTarget().GetTargetPath())},
            {"configs", aggregated_configs},
        });
    env::Command::Execute(command_str);
  }

  // - Clang Compile Commands
  plugin::ClangCompileCommands({&build_setup.GetUserTarget()}).Generate();

  return 0;
}

static void clean_cb() {
  env::log_info(kTag, fmt::format("Cleaning {}", env::get_project_build_dir()));
  fs::remove_all(env::get_project_build_dir());
}
