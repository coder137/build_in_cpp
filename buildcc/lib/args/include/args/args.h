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

#ifndef ARGS_ARGS_H_
#define ARGS_ARGS_H_

#include <filesystem>

// Third Party
#include "CLI/CLI.hpp"

// BuildCC
#include "env/logging.h"

#include "toolchain/toolchain.h"

#include "target/common/target_config.h"

namespace fs = std::filesystem;

namespace buildcc {

/**
 * @brief Toolchain State used to selectively build and test targets
 */
struct ArgToolchainState {
  bool build{false};
  bool test{false};
};

/**
 * @brief Toolchain Arg used to receive toolchain information through the
 * command line
 * Bundled with Toolchain State
 */
struct ArgToolchain {
  ArgToolchain(){};
  ArgToolchain(ToolchainId initial_id, const std::string &initial_name,
               const ToolchainExecutables &initial_executables)
      : id(initial_id), name(initial_name), executables(initial_executables) {}
  ArgToolchain(ToolchainId initial_id, const std::string &initial_name,
               const std::string &initial_assembler,
               const std::string &initial_c_compiler,
               const std::string &initial_cpp_compiler,
               const std::string &initial_archiver,
               const std::string &initial_linker)
      : ArgToolchain(initial_id, initial_name,
                     ToolchainExecutables(initial_assembler, initial_c_compiler,
                                          initial_cpp_compiler,
                                          initial_archiver, initial_linker)) {}

  /**
   * @brief Construct a BaseToolchain from the arguments supplied through the
   * command line information
   */
  // TODO, Update this for lock and ToolchainConfig
  BaseToolchain ConstructToolchain() const {
    return BaseToolchain(id, name, executables);
  }

  ArgToolchainState state;
  ToolchainId id{ToolchainId::Undefined};
  std::string name{""};
  ToolchainExecutables executables;
};

// NOTE, Incomplete without pch_compile_command
// TODO, Update this for PCH
struct ArgTarget {
  ArgTarget(){};

  TargetConfig GetTargetConfig() {
    TargetConfig config;
    config.compile_command = compile_command;
    config.link_command = link_command;
    return config;
  }

  std::string compile_command;
  std::string link_command;
};

struct ArgCustom {
  virtual void Add(CLI::App &app) = 0;
};

class Args {
private:
  class Instance;
  struct Internal;

public:
  Args() = delete;
  Args(const Args &) = delete;
  Args(Args &&) = delete;

  static Instance &Init();
  static void Deinit();

  // Getters
  static bool IsInit();
  static bool IsParsed();
  static bool Clean();
  static env::LogLevel GetLogLevel();

  static const fs::path &GetProjectRootDir();
  static const fs::path &GetProjectBuildDir();

private:
  static void RootArgs();
  static Internal &RefInternal();
  static CLI::App &RefApp();

private:
  static std::unique_ptr<Internal> internal_;
};

class Args::Instance {
public:
  /**
   * @brief Parse command line information to CLI11
   *
   * @param argc from int main(int argc, char ** argv)
   * @param argv from int main(int argc, char ** argv)
   */
  static void Parse(int argc, const char *const *argv);

  /**
   * @brief Add toolchain with a unique name and description
   *
   * @param out Receive the toolchain information through the CLI
   * @param initial Set the default toolchain information as a fallback
   */
  Instance &AddToolchain(const std::string &name,
                         const std::string &description, ArgToolchain &out,
                         const ArgToolchain &initial = ArgToolchain());

  /**
   * @brief Add toolchain with a unique name and description
   *
   * @param out Receive the toolchain information through the CLI
   * @param initial Set the default toolchain information as a fallback
   */
  Instance &AddTarget(const std::string &name, const std::string &description,
                      ArgTarget &out, const ArgTarget &initial = ArgTarget());

  /**
   * @brief Custom callback for data
   *
   * @param add_cb Add callback that exposes underlying CLI::App
   */
  Instance &AddCustomCallback(const std::function<void(CLI::App &)> &add_cb);

  /**
   * @brief Add custom data
   *
   * @param data Derive from `buildcc::ArgCustom` and override the `Add` API
   */
  Instance &AddCustomData(ArgCustom &data);
};

struct Args::Internal {
  Instance instance;
  CLI::App app{"BuildCC Buildsystem"};
  CLI::App *toolchain{nullptr};
  CLI::App *target{nullptr};
};

} // namespace buildcc

#endif
