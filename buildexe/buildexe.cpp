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

static fs::path get_env_buildcc_home();

static void user_output_target_cb(BaseTarget &target,
                                  const ArgTargetInputs &inputs);

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

  PersistentStorage storage;
  Target_generic user_output_target(out_targetinfo.name, out_targetinfo.type,
                                    toolchain,
                                    TargetEnv(out_targetinfo.relative_to_root));
  if (out_mode == BuildExeMode::Script) {
    // Compile buildcc using the constructed toolchain
    fs::path buildcc_home = get_env_buildcc_home();
    auto &buildcc_package = storage.Add<BuildBuildCC>(
        "BuildccPackage", reg, toolchain,
        TargetEnv(buildcc_home / "buildcc",
                  buildcc_home / "buildcc" / "_build_exe"));
    buildcc_package.Setup(host_toolchain_arg.state);

    // Add buildcc as a dependency to user_output_target
    user_output_target.AddLibDep(buildcc_package.GetBuildcc());
    user_output_target.Insert(buildcc_package.GetBuildcc(),
                              {
                                  SyncOption::PreprocessorFlags,
                                  SyncOption::CppCompileFlags,
                                  SyncOption::IncludeDirs,
                                  SyncOption::LinkFlags,
                                  SyncOption::HeaderFiles,
                                  SyncOption::IncludeDirs,
                                  SyncOption::LibDeps,
                                  SyncOption::ExternalLibDeps,
                              });
    switch (toolchain.GetId()) {
    case ToolchainId::MinGW:
      user_output_target.AddLinkFlag("-Wl,--allow-multiple-definition");
      break;
    default:
      break;
    }
  }

  ArgToolchainState user_output_state;
  user_output_state.build = true;
  reg.Build(user_output_state, user_output_target_cb, user_output_target,
            out_targetinputs);

  if (out_mode == BuildExeMode::Script) {
    auto &buildcc_package = storage.Ref<BuildBuildCC>("BuildccPackage");
    reg.Dep(user_output_target, buildcc_package.GetBuildcc());
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
             fmt::format("{}", user_output_target.GetTargetPath())},
            {"configs", aggregated_configs},
        });
    env::Command::Execute(command_str);
  }

  // - Clang Compile Commands
  plugin::ClangCompileCommands({&user_output_target}).Generate();

  return 0;
}

static void clean_cb() {
  env::log_info(kTag, fmt::format("Cleaning {}", env::get_project_build_dir()));
  fs::remove_all(env::get_project_build_dir());
}

static fs::path get_env_buildcc_home() {
  const char *buildcc_home = getenv("BUILDCC_HOME");
  env::assert_fatal(buildcc_home != nullptr,
                    "BUILDCC_HOME environment variable not defined");

  // NOTE, Verify BUILDCC_HOME
  // auto &buildcc_path = storage.Add<fs::path>("buildcc_path", buildcc_home);
  fs::path buildcc_home_path{buildcc_home};
  env::assert_fatal(fs::exists(buildcc_home_path),
                    "{BUILDCC_HOME} path not found path not found");
  env::assert_fatal(fs::exists(buildcc_home_path / "buildcc"),
                    "{BUILDCC_HOME}/buildcc path not found");
  env::assert_fatal(fs::exists(buildcc_home_path / "libs"),
                    "{BUILDCC_HOME}/libs path not found");
  env::assert_fatal(fs::exists(buildcc_home_path / "extensions"),
                    "{BUILDCC_HOME}/extensions path not found");

  return buildcc_home_path;
}

static void user_output_target_cb(BaseTarget &target,
                                  const ArgTargetInputs &inputs) {
  for (const auto &s : inputs.source_files) {
    target.AddSource(s);
  }
  for (const auto &i : inputs.include_dirs) {
    target.AddIncludeDir(i);
  }

  for (const auto &l : inputs.lib_dirs) {
    target.AddLibDir(l);
  }
  for (const auto &el : inputs.external_lib_deps) {
    target.AddLibDep(el);
  }

  for (const auto &flag : inputs.preprocessor_flags) {
    target.AddPreprocessorFlag(flag);
  }
  for (const auto &flag : inputs.common_compile_flags) {
    target.AddCommonCompileFlag(flag);
  }
  for (const auto &flag : inputs.asm_compile_flags) {
    target.AddAsmCompileFlag(flag);
  }
  for (const auto &flag : inputs.c_compile_flags) {
    target.AddCCompileFlag(flag);
  }
  for (const auto &flag : inputs.cpp_compile_flags) {
    target.AddCppCompileFlag(flag);
  }
  for (const auto &flag : inputs.link_flags) {
    target.AddLinkFlag(flag);
  }

  target.Build();
}
