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

#include "bootstrap/build_buildcc.h"
#include "bootstrap/build_cli11.h"
#include "bootstrap/build_flatbuffers.h"
#include "bootstrap/build_fmtlib.h"
#include "bootstrap/build_spdlog.h"
#include "bootstrap/build_taskflow.h"
#include "bootstrap/build_tpl.h"

using namespace buildcc;

constexpr const char *const kTag = "BuildExe";

enum class BuildExeMode {
  Immediate,
  Script,
};

struct ArgTargetInfo {
  std::string name;
  TargetType type;
  fs::path relative_to_root;
};

struct ArgTargetInputs {
  // Sources
  std::vector<fs::path> source_files;
  std::vector<fs::path> include_dirs;

  // External libs
  std::vector<fs::path> lib_dirs;
  std::vector<std::string> external_lib_deps;

  // Flags
  std::vector<std::string> preprocessor_flags;
  std::vector<std::string> common_compile_flags;
  std::vector<std::string> asm_compile_flags;
  std::vector<std::string> c_compile_flags;
  std::vector<std::string> cpp_compile_flags;
  std::vector<std::string> link_flags;
};

struct ArgScriptInfo {
  std::vector<std::string> configs;
};

static const std::unordered_map<const char *, TargetType> kTargetTypeMap{
    {"executable", TargetType::Executable},
    {"staticLibrary", TargetType::StaticLibrary},
    {"dynamicLibrary", TargetType::DynamicLibrary},
};

static const std::unordered_map<const char *, BuildExeMode> kBuildExeModeMap{
    {"immediate", BuildExeMode::Immediate},
    {"script", BuildExeMode::Script},
};

static void clean_cb();

static void setup_arg_target_info(Args &args, ArgTargetInfo &out);
static void setup_arg_target_inputs(Args &args, ArgTargetInputs &out);
static void setup_arg_script_mode(Args &args, ArgScriptInfo &out);

static void host_toolchain_verify(const BaseToolchain &toolchain);

static fs::path get_env_buildcc_home();

static void user_output_target_cb(BaseTarget &target,
                                  const ArgTargetInputs &inputs);

// TODO, Add BuildExeMode::Script usage
// TODO, Refactor magic strings
int main(int argc, char **argv) {
  Args args;

  ArgToolchain custom_toolchain_arg;
  args.AddToolchain("host", "Host Toolchain", custom_toolchain_arg);

  // TODO, Add Verification subcommand here for OS, Compiler etc!
  // os win, linux considerations
  // compiler gcc, msvc considerations
  // arch considerations

  BuildExeMode mode;
  args.Ref()
      .add_option("--mode", mode, "Provide BuildExe run mode")
      ->transform(CLI::CheckedTransformer(kBuildExeModeMap, CLI::ignore_case))
      ->required();

  ArgTargetInfo out_targetinfo;
  setup_arg_target_info(args, out_targetinfo);

  ArgTargetInputs out_targetinputs;
  setup_arg_target_inputs(args, out_targetinputs);

  ArgScriptInfo out_scriptinfo;
  setup_arg_script_mode(args, out_scriptinfo);

  // script mode specific arguments

  // TODO, Add buildcc (git cloned)
  // TODO, Add libraries (git cloned)
  // TODO, Add extension (git cloned)

  args.Parse(argc, argv);

  Register reg(args);
  reg.Clean(clean_cb);

  // Build
  BaseToolchain toolchain = custom_toolchain_arg.ConstructToolchain();

  // BaseToolchain toolchain = custom_toolchain_arg.ConstructToolchain();
  auto verified_toolchains = toolchain.Verify();
  env::assert_fatal(!verified_toolchains.empty(),
                    "Toolchain could not be verified. Please input correct "
                    "Gcc, Msvc, Clang or MinGW toolchain executable names");
  if (verified_toolchains.size() > 1) {
    env::log_info(
        kTag,
        fmt::format(
            "Found {} toolchains. By default using the first added"
            "toolchain. Modify your environment `PATH` information if you "
            "would like compiler precedence when similar compilers are "
            "detected in different folders",
            verified_toolchains.size()));
  }

  // Print
  int counter = 1;
  for (const auto &vt : verified_toolchains) {
    std::string info = fmt::format("{}. : {}", counter, vt.ToString());
    env::log_info("Host Toolchain", info);
    counter++;
  }

  // TODO, Update Toolchain with VerifiedToolchain
  // toolchain.UpdateFrom(verified_toolchain);

  if (mode == BuildExeMode::Script) {
    host_toolchain_verify(toolchain);
  }

  PersistentStorage storage;
  Target_generic user_output_target(out_targetinfo.name, out_targetinfo.type,
                                    toolchain,
                                    TargetEnv(out_targetinfo.relative_to_root));
  if (mode == BuildExeMode::Script) {
    // Compile buildcc using the constructed toolchain
    fs::path buildcc_home = get_env_buildcc_home();
    auto &buildcc_package = storage.Add<BuildBuildCC>(
        "BuildccPackage", reg, toolchain,
        TargetEnv(buildcc_home / "buildcc",
                  buildcc_home / "buildcc" / "_build_exe"));
    buildcc_package.Setup(custom_toolchain_arg.state);

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

  reg.Build(custom_toolchain_arg.state, user_output_target_cb,
            user_output_target, out_targetinputs);

  if (mode == BuildExeMode::Script) {
    auto &buildcc_package = storage.Ref<BuildBuildCC>("BuildccPackage");
    reg.Dep(user_output_target, buildcc_package.GetBuildcc());
  }

  // Runners
  reg.RunBuild();

  // Run
  if (mode == BuildExeMode::Script) {
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

// TODO, Add subcommand [build.info]
static void setup_arg_target_info(Args &args, ArgTargetInfo &out) {
  auto &app = args.Ref();

  app.add_option("--name", out.name, "Provide Target name")->required();

  app.add_option("--type", out.type, "Provide Target Type")
      ->transform(CLI::CheckedTransformer(kTargetTypeMap, CLI::ignore_case))
      ->required();

  app.add_option("--relative_to_root", out.relative_to_root,
                 "Provide Target relative to root")
      ->required();
}

// TODO, Add subcommand [build.inputs]
// TODO, Add group, group by sources, headers, inncludes on CLI
static void setup_arg_target_inputs(Args &args, ArgTargetInputs &out) {
  auto &app = args.Ref();

  app.add_option("--srcs", out.source_files, "Provide source files");
  app.add_option("--includes", out.include_dirs, "Provide include dirs");

  app.add_option("--lib_dirs", out.lib_dirs, "Provide lib dirs");
  app.add_option("--external_libs", out.external_lib_deps,
                 "Provide external libs");

  app.add_option("--preprocessor_flags", out.preprocessor_flags,
                 "Provide Preprocessor flags");
  app.add_option("--common_compile_flags", out.common_compile_flags,
                 "Provide CommonCompile Flags");
  app.add_option("--asm_compile_flags", out.asm_compile_flags,
                 "Provide AsmCompile Flags");
  app.add_option("--c_compile_flags", out.c_compile_flags,
                 "Provide CCompile Flags");
  app.add_option("--cpp_compile_flags", out.cpp_compile_flags,
                 "Provide CppCompile Flags");
  app.add_option("--link_flags", out.link_flags, "Provide Link Flags");
}

static void setup_arg_script_mode(Args &args, ArgScriptInfo &out) {
  auto *script_args = args.Ref().add_subcommand("script");
  script_args->add_option("--configs", out.configs,
                          "Config files for script mode");
}

static void host_toolchain_verify(const BaseToolchain &toolchain) {
  env::log_info(kTag, "*** Starting Toolchain verification ***");

  fs::path file = env::get_project_build_dir() / "verify_host_toolchain" /
                  "verify_host_toolchain.cpp";
  fs::create_directories(file.parent_path());
  std::string file_data = R"(// Generated by BuildExe
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

int main() {
  std::cout << "Verifying host toolchain" << std::endl;
  std::cout << "Current Path: " << fs::current_path() << std::endl;
  return 0;
})";
  env::save_file(path_as_string(file).c_str(), file_data, false);

  ExecutableTarget_generic target(
      "verify", toolchain, TargetEnv(file.parent_path(), file.parent_path()));

  target.AddSource(file);
  switch (toolchain.GetId()) {
  case ToolchainId::Gcc:
  case ToolchainId::MinGW:
    target.AddCppCompileFlag("-std=c++17");
    break;
  case ToolchainId::Msvc:
    target.AddCppCompileFlag("/std:c++17");
    break;
  default:
    env::assert_fatal<false>("Invalid Compiler Id");
  }
  target.Build();

  // Build
  tf::Executor executor;
  executor.run(target.GetTaskflow());
  executor.wait_for_all();
  env::assert_fatal(env::get_task_state() == env::TaskState::SUCCESS,
                    "Input toolchain could not compile host program. "
                    "Requires HOST toolchain");

  // Run
  bool execute = env::Command::Execute(fmt::format(
      "{executable}", fmt::arg("executable", target.GetTargetPath().string())));
  env::assert_fatal(execute, "Could not execute verification target");
  env::log_info(kTag, "*** Toolchain verification done ***");
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
