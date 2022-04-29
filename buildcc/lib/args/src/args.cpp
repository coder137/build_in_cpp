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

#include "args/args.h"

namespace {

// Error messages
constexpr const char *const kArgsNotInit =
    "Initialize Args using the Args::Init API";

// Groups
constexpr const char *const kRootGroup = "Root";

// Options & Flags
constexpr const char *const kHelpAllParam = "--help-all";
constexpr const char *const kHelpAllDesc = "Expand individual options.";

constexpr const char *const kConfigParam = "--config";
constexpr const char *const kConfigDesc = "Read <config>.toml files.";
constexpr int kMinFiles = 0;
constexpr int kMaxFiles = 10;

constexpr const char *const kCleanParam = "--clean";
constexpr const char *const kCleanDesc = "Clean artifacts";

constexpr const char *const kLoglevelParam = "--loglevel";
constexpr const char *const kLoglevelDesc = "LogLevel settings";

constexpr const char *const kRootDirParam = "--root_dir";
constexpr const char *const kRootDirDesc =
    "Project root directory (relative to current directory)";

constexpr const char *const kBuildDirParam = "--build_dir";
constexpr const char *const kBuildDirDesc =
    "Project build dir (relative to current directory)";

// Subcommands
constexpr const char *const kToolchainSubcommand = "toolchain";
constexpr const char *const kToolchainDesc = "Select Toolchain";
constexpr const char *const kToolchainGroup = "Supported Toolchains";
constexpr const char *const kToolchainIdDesc = "Toolchain ID settings";

constexpr const char *const kToolchainBuildParam = "--build";
constexpr const char *const kToolchainTestParam = "--test";
constexpr const char *const kToolchainIdParam = "--id";
constexpr const char *const kToolchainNameParam = "--name";
constexpr const char *const kToolchainAsmCompilerParam = "--asm_compiler";
constexpr const char *const kToolchainCCompilerParam = "--c_compiler";
constexpr const char *const kToolchainCppCompilerParam = "--cpp_compiler";
constexpr const char *const kToolchainArchiverParam = "--archiver";
constexpr const char *const kToolchainLinkerParam = "--linker";

constexpr const char *const kTargetSubcommand = "target";
constexpr const char *const kTargetDesc = "Select Target";
constexpr const char *const kTargetGroup = "Custom Targets";

constexpr const char *const kTargetCompileCommandParam = "--compile_command";
constexpr const char *const kTargetLinkCommandParam = "--link_command";

const std::unordered_map<const char *, buildcc::env::LogLevel> kLogLevelMap{
    {"trace", buildcc::env::LogLevel::Trace},
    {"debug", buildcc::env::LogLevel::Debug},
    {"info", buildcc::env::LogLevel::Info},
    {"warning", buildcc::env::LogLevel::Warning},
    {"critical", buildcc::env::LogLevel::Critical},
};

const std::unordered_map<const char *, buildcc::ToolchainId> kToolchainIdMap{
    {"gcc", buildcc::ToolchainId::Gcc},
    {"msvc", buildcc::ToolchainId::Msvc},
    {"mingw", buildcc::ToolchainId::MinGW},
    {"clang", buildcc::ToolchainId::Clang},
    {"custom", buildcc::ToolchainId::Custom},
    {"undefined", buildcc::ToolchainId::Undefined},
};

// Static variables
bool clean_{false};
buildcc::env::LogLevel loglevel_{buildcc::env::LogLevel::Info};
fs::path project_root_dir_{""};
fs::path project_build_dir_{"_internal"};

} // namespace

namespace buildcc {

std::unique_ptr<Args::Internal> Args::internal_;

Args::Instance &Args::Init() {
  if (!internal_) {
    internal_ = std::make_unique<Internal>();
    auto &app = RefApp();
    internal_->toolchain =
        app.add_subcommand(kToolchainSubcommand, kToolchainDesc);
    internal_->target = app.add_subcommand(kTargetSubcommand, kTargetDesc);
    RootArgs();
  }
  return internal_->instance;
}

void Args::Deinit() { internal_.reset(nullptr); }

bool Args::IsInit() { return static_cast<bool>(internal_); }
bool Args::IsParsed() {
  if (!IsInit()) {
    return false;
  }
  return RefApp().parsed();
}
bool Args::Clean() { return clean_; }
env::LogLevel Args::GetLogLevel() { return loglevel_; }

const fs::path &Args::GetProjectRootDir() { return project_root_dir_; }
const fs::path &Args::GetProjectBuildDir() { return project_build_dir_; }

// Private

void Args::RootArgs() {
  auto &app = RefApp();
  app.set_help_all_flag(kHelpAllParam, kHelpAllDesc);

  app.set_config(kConfigParam, "", kConfigDesc)->expected(kMinFiles, kMaxFiles);

  // Root flags
  auto *root_group = app.add_option_group(kRootGroup);

  root_group->add_flag(kCleanParam, clean_, kCleanDesc);
  root_group->add_option(kLoglevelParam, loglevel_, kLoglevelDesc)
      ->transform(CLI::CheckedTransformer(kLogLevelMap, CLI::ignore_case));

  // Dir flags
  root_group->add_option(kRootDirParam, project_root_dir_, kRootDirDesc)
      ->required();
  root_group->add_option(kBuildDirParam, project_build_dir_, kBuildDirDesc)
      ->required();
}

Args::Internal &Args::RefInternal() {
  env::assert_fatal(internal_ != nullptr, kArgsNotInit);
  return *internal_;
}
CLI::App &Args::RefApp() { return RefInternal().app; }

// ArgsToolchain

std::optional<ToolchainCompilerInfo>
ArgToolchain::GetToolchainInfo(const ToolchainExecutables &exes) const {
  switch (id) {
  case ToolchainId::Gcc:
  case ToolchainId::MinGW:
  case ToolchainId::Msvc:
  case ToolchainId::Clang:
    return GlobalToolchainInfo::Get(id)(exes);
    break;
  case ToolchainId::Custom:
    ASSERT_FATAL(GetToolchainInfoCb(), "For ToolchainId::Custom "
                                       "add ToolchainInfoCb using the "
                                       "Toolchain::SetToolchainInfoCb API");
    return GetToolchainInfoCb()(exes);
    break;
  default:
    break;
  }

  return {};
}

// Args::Instance

/**
 * @brief Add toolchain with a unique name and description
 *
 * @param out Receive the toolchain information through the CLI
 * @param initial Set the default toolchain information as a fallback
 */
Args::Instance &Args::Instance::AddToolchain(const std::string &name,
                                             const std::string &description,
                                             ArgToolchain &out,
                                             const ArgToolchain &initial) {
  CLI::App *toolchain = RefInternal().toolchain;
  CLI::App *t_user =
      toolchain->add_subcommand(name, description)->group(kToolchainGroup);

  // State
  t_user->add_flag(kToolchainBuildParam, out.state.build);
  t_user->add_flag(kToolchainTestParam, out.state.test);

  // Id, Name, Executables
  t_user->add_option(kToolchainIdParam, out.id, kToolchainIdDesc)
      ->transform(CLI::CheckedTransformer(kToolchainIdMap, CLI::ignore_case))
      ->default_val(initial.id);
  t_user->add_option(kToolchainNameParam, out.name)->default_val(initial.name);
  t_user->add_option(kToolchainAsmCompilerParam, out.executables.assembler)
      ->default_val(initial.executables.assembler);
  t_user->add_option(kToolchainCCompilerParam, out.executables.c_compiler)
      ->default_val(initial.executables.c_compiler);
  t_user->add_option(kToolchainCppCompilerParam, out.executables.cpp_compiler)
      ->default_val(initial.executables.cpp_compiler);
  t_user->add_option(kToolchainArchiverParam, out.executables.archiver)
      ->default_val(initial.executables.archiver);
  t_user->add_option(kToolchainLinkerParam, out.executables.linker)
      ->default_val(initial.executables.linker);

  // TODO, Add toolchain config
  return *this;
}

/**
 * @brief Add toolchain with a unique name and description
 *
 * @param out Receive the toolchain information through the CLI
 * @param initial Set the default toolchain information as a fallback
 */
Args::Instance &Args::Instance::AddTarget(const std::string &name,
                                          const std::string &description,
                                          ArgTarget &out,
                                          const ArgTarget &initial) {
  CLI::App *target = RefInternal().target;
  CLI::App *targetuser =
      target->add_subcommand(name, description)->group(kTargetGroup);
  targetuser->add_option(kTargetCompileCommandParam, out.compile_command)
      ->default_val(initial.compile_command);
  targetuser->add_option(kTargetLinkCommandParam, out.link_command)
      ->default_val(initial.link_command);
  return *this;
}

Args::Instance &Args::Instance::AddCustomCallback(
    const std::function<void(CLI::App &)> &add_cb) {
  auto &app = RefApp();
  add_cb(app);
  return *this;
}

Args::Instance &Args::Instance::AddCustomData(ArgCustom &data) {
  auto &app = RefApp();
  data.Add(app);
  return *this;
}

} // namespace buildcc
