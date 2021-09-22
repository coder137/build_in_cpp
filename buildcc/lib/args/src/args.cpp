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

#include "args/args.h"

namespace {

constexpr int kMinFiles = 0;
constexpr int kMaxFiles = INT_MAX;

} // namespace

namespace buildcc {

void Args::AddCustomToolchain(const std::string &name,
                              const std::string &description, ToolchainArg &out,
                              const ToolchainArg &initial) {
  CLI::App *t_user = AddToolchain(name, description, "Custom", out.state);

  t_user->add_option("--id", out.id, "Toolchain ID settings")
      ->transform(CLI::CheckedTransformer(toolchain_id_map_, CLI::ignore_case))
      ->default_val(initial.id);
  t_user->add_option("--name", out.name)->default_val(initial.name);
  t_user->add_option("--asm_compiler", out.asm_compiler)
      ->default_val(initial.asm_compiler);
  t_user->add_option("--c_compiler", out.c_compiler)
      ->default_val(initial.c_compiler);
  t_user->add_option("--cpp_compiler", out.cpp_compiler)
      ->default_val(initial.cpp_compiler);
  t_user->add_option("--archiver", out.archiver)->default_val(initial.archiver);
  t_user->add_option("--linker", out.linker)->default_val(initial.linker);
}

void Args::AddCustomTarget(const std::string &name,
                           const std::string &description, TargetArg &out,
                           const TargetArg &initial) {
  CLI::App *target_user =
      target_->add_subcommand(name, description)->group("Custom");
  target_user->add_option("--compile_command", out.compile_command)
      ->default_val(initial.compile_command);
  target_user->add_option("--link_command", out.link_command)
      ->default_val(initial.link_command);
}

// Private

void Args::Initialize() { RootArgs(); }

void Args::RootArgs() {
  app_.set_help_all_flag("--help-all", "Expand individual options");

  app_.set_config("--config", "", "Read a <config>.toml file")
      ->expected(kMinFiles, kMaxFiles);

  // Root flags
  app_.add_flag("--clean", clean_, "Clean artifacts")->group("Root");
  app_.add_option("--loglevel", loglevel_, "LogLevel settings")
      ->transform(CLI::CheckedTransformer(loglevel_map_, CLI::ignore_case))
      ->group("Root");

  // Dir flags
  app_.add_option("--root_dir", project_root_dir_,
                  "Project root directory (relative to current directory)")
      ->required()
      ->group("Root");
  app_.add_option("--build_dir", project_build_dir_,
                  "Project build dir (relative to current directory)")
      ->required()
      ->group("Root");
}

void Args::CommonToolchainArgs() {
  toolchain_ = app_.add_subcommand("toolchain", "Select Toolchain");
  (void)AddToolchain("gcc", "GNU GCC Toolchain", "Supported", gcc_state_);
  (void)AddToolchain("msvc", "MSVC Toolchain", "Supported", msvc_state_);
}

void Args::CommonTargetArgs() {
  target_ = app_.add_subcommand("target", "Select target");
}

CLI::App *Args::AddToolchain(const std::string &name,
                             const std::string &description,
                             const std::string &group,
                             ToolchainState &toolchain_state) {
  CLI::App *t_user =
      toolchain_->add_subcommand(name, description)->group(group);
  t_user->add_flag("-b,--build", toolchain_state.build);
  t_user->add_flag("-t,--test", toolchain_state.test);
  return t_user;
}

} // namespace buildcc
