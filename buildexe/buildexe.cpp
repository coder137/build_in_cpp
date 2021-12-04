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

static void clean_cb();

static void
user_output_target_cb(BaseTarget &target, const std::vector<std::string> &srcs,
                      const std::vector<std::string> &includes,
                      const std::vector<std::string> &external_libs);

int main(int argc, char **argv) {
  Args args;

  ArgToolchain custom_toolchain_arg;
  args.AddToolchain("custom", "Host Toolchain", custom_toolchain_arg);

  std::string target_name_arg;
  args.Ref()
      .add_option("--name", target_name_arg, "Provide Target name")
      ->required();

  TargetType target_type_arg;
  const std::unordered_map<const char *, TargetType> kTargetTypeMap{
      {"executable", TargetType::Executable},
      {"staticLibrary", TargetType::StaticLibrary},
      {"dynamicLibrary", TargetType::DynamicLibrary},
  };
  args.Ref()
      .add_option("--type", target_type_arg, "Provide Target Type")
      ->transform(CLI::CheckedTransformer(kTargetTypeMap, CLI::ignore_case))
      ->required();

  std::string target_relative_to_root_arg;
  args.Ref()
      .add_option("--relative_to_root", target_relative_to_root_arg,
                  "Provide Target relative to root")
      ->required();

  std::vector<std::string> srcs;
  std::vector<std::string> includes;
  std::vector<std::string> external_libs;

  args.Ref().add_option("--srcs", srcs, "Compile source files");
  args.Ref().add_option("--includes", includes, "Add include paths");

  // std::string core;
  // args.Ref().add_option("--core", core, "Select core buildcc version");

  // TODO, Add base (git cloned raw versions)
  // TODO, Add libraries (compiled version of code! with libs and header
  // linkage options)
  // TODO, Add extension
  args.Parse(argc, argv);

  Register reg(args);
  reg.Clean(clean_cb);

  // Build
  BaseToolchain toolchain = custom_toolchain_arg.ConstructToolchain();
  Target_generic user_output_target(target_name_arg, target_type_arg, toolchain,
                                    TargetEnv(target_relative_to_root_arg));
  reg.Build(custom_toolchain_arg.state, user_output_target_cb,
            user_output_target, srcs, includes, external_libs);

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

static void
user_output_target_cb(BaseTarget &target, const std::vector<std::string> &srcs,
                      const std::vector<std::string> &includes,
                      const std::vector<std::string> &external_libs) {
  for (const auto &s : srcs) {
    target.AddSource(s);
  }

  for (const auto &i : includes) {
    target.AddIncludeDir(i, true);
  }

  for (const auto &el : external_libs) {
    target.AddLibDep(el);
  }

  // TODO, Compile buildcc here and add it!
  // target.AddLibDep()

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
