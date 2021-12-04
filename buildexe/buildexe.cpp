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

struct ArgTargetStorer {
  std::vector<fs::path> source_files;
  std::vector<fs::path> include_dirs;
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
static void setup_arg_target_storer(Args &args, ArgTargetStorer &out);

static void user_output_target_cb(BaseTarget &target,
                                  const ArgTargetStorer &storer);

// TODO, Add BuildExeMode::Script usage
int main(int argc, char **argv) {
  Args args;

  ArgToolchain custom_toolchain_arg;
  args.AddToolchain("custom", "Host Toolchain", custom_toolchain_arg);

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

  ArgTargetStorer out_targetstorer;
  setup_arg_target_storer(args, out_targetstorer);

  // TODO, Add base (git cloned raw versions)
  // TODO, Add libraries (compiled version of code! with libs and header
  // linkage options)
  // TODO, Add extension

  args.Parse(argc, argv);

  Register reg(args);
  reg.Clean(clean_cb);

  // Build
  BaseToolchain toolchain = custom_toolchain_arg.ConstructToolchain();
  // TODO, Verify toolchain by compiling a dummy program

  Target_generic user_output_target(out_targetinfo.name, out_targetinfo.type,
                                    toolchain,
                                    TargetEnv(out_targetinfo.relative_to_root));
  // if (mode == BuildExeMode::Script) {
  // TODO, Compile BuildCC here (and make persistent)
  // NOTE, Add to user_output_target
  //   reg.Callback([&]() { user_output_target.AddLibDep(libbuildcc); });
  // }
  reg.Build(custom_toolchain_arg.state, user_output_target_cb,
            user_output_target, out_targetstorer);

  // Runners
  reg.RunBuild();

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
static void setup_arg_target_storer(Args &args, ArgTargetStorer &out) {
  auto &app = args.Ref();

  app.add_option("--srcs", out.source_files, "Compile source files");
  app.add_option("--includes", out.include_dirs, "Add include paths");
  // TODO, Add more options here
}

static void user_output_target_cb(BaseTarget &target,
                                  const ArgTargetStorer &storer) {
  for (const auto &s : storer.source_files) {
    target.AddSource(s);
  }

  for (const auto &i : storer.include_dirs) {
    target.AddIncludeDir(i, true);
  }

  // * NOTE, Add your own CPP optimization flags depending on toolchain!
  switch (target.GetToolchain().GetId()) {
  case ToolchainId::Gcc:
    target.AddCppCompileFlag("-std=c++17");
    target.AddCppCompileFlag("-Os");
    break;
  case ToolchainId::Msvc:
    target.AddCppCompileFlag("/std:c++17");
    target.AddCppCompileFlag("/Ot");
    break;
  default:
    env::assert_fatal<false>("ToolchainId not currently supported");
  }

  target.Build();
}
