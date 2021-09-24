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

// Groups
constexpr const char *const kRootGroup = "Root";

// Options & Flags
constexpr const char *const kHelpAllFlag = "--help-all";
constexpr const char *const kHelpAllDesc = "Expand individual options.";

constexpr const char *const kConfigFlag = "--config";
constexpr const char *const kConfigDesc = "Read <config>.toml files.";
constexpr int kMinFiles = 0;
constexpr int kMaxFiles = 10;

constexpr const char *const kCleanFlag = "--clean";
constexpr const char *const kCleanDesc = "Clean artifacts";

constexpr const char *const kLoglevelFlag = "--loglevel";
constexpr const char *const kLoglevelDesc = "LogLevel settings";

constexpr const char *const kRootDirFlag = "--root_dir";
constexpr const char *const kRootDirDesc =
    "Project root directory (relative to current directory)";

constexpr const char *const kBuildDirFlag = "--build_dir";
constexpr const char *const kBuildDirDesc =
    "Project build dir (relative to current directory)";

// Subcommands
constexpr const char *const kToolchainSubcommand = "toolchain";
constexpr const char *const kToolchainDesc = "Select Toolchain";

constexpr const char *const kTargetSubcommand = "target";
constexpr const char *const kTargetDesc = "Select Target";

const std::unordered_map<const char *, buildcc::env::LogLevel> kLogLevelMap{
    {"trace", buildcc::env::LogLevel::Trace},
    {"debug", buildcc::env::LogLevel::Debug},
    {"info", buildcc::env::LogLevel::Info},
    {"warning", buildcc::env::LogLevel::Warning},
    {"critical", buildcc::env::LogLevel::Critical},
};

const std::unordered_map<const char *, buildcc::base::Toolchain::Id>
    kToolchainIdMap{
        {"gcc", buildcc::base::Toolchain::Id::Gcc},
        {"msvc", buildcc::base::Toolchain::Id::Msvc},
        {"clang", buildcc::base::Toolchain::Id::Clang},
        {"custom", buildcc::base::Toolchain::Id::Custom},
        {"undefined", buildcc::base::Toolchain::Id::Undefined},
    };

} // namespace

namespace buildcc {

void Args::AddCustomToolchain(const std::string &name,
                              const std::string &description, ToolchainArg &out,
                              const ToolchainArg &initial) {
  CLI::App *t_user = AddToolchain(name, description, "Custom", out.state);

  t_user->add_option("--id", out.id, "Toolchain ID settings")
      ->transform(CLI::CheckedTransformer(kToolchainIdMap, CLI::ignore_case))
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

void Args::Initialize() {
  RootArgs();
  toolchain_ = app_.add_subcommand(kToolchainSubcommand, kToolchainDesc);
  target_ = app_.add_subcommand(kTargetSubcommand, kTargetDesc);
}

void Args::RootArgs() {
  app_.set_help_all_flag(kHelpAllFlag, kHelpAllDesc);

  app_.set_config(kConfigFlag, "", kConfigDesc)->expected(kMinFiles, kMaxFiles);

  // Root flags

  app_.add_flag(kCleanFlag, clean_, kCleanDesc)->group(kRootGroup);
  app_.add_option(kLoglevelFlag, loglevel_, kLoglevelDesc)
      ->transform(CLI::CheckedTransformer(kLogLevelMap, CLI::ignore_case))
      ->group(kRootGroup);

  // Dir flags
  app_.add_option(kRootDirFlag, project_root_dir_, kRootDirDesc)
      ->required()
      ->group(kRootGroup);
  app_.add_option(kBuildDirFlag, project_build_dir_, kBuildDirDesc)
      ->required()
      ->group(kRootGroup);
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
