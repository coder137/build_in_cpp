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

namespace fs = std::filesystem;

namespace buildcc {

/**
 * @brief Toolchain State used by the Register module to selectively build or
 * test targets
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

  std::string compile_command{""};
  std::string link_command{""};
};

class Args {
public:
public:
  Args() = delete;
  Args(const Args &) = delete;
  Args(Args &&) = delete;

  static void Init();
  static void Deinit();

  /**
   * @brief Parse command line information to CLI11
   *
   * @param argc from int main(int argc, char ** argv)
   * @param argv from int main(int argc, char ** argv)
   */
  static void Parse(int argc, const char *const *argv);

  /**
   * @brief Modifiable reference to CLI::App (CLI11)
   */
  static CLI::App &Ref();

  /**
   * @brief Constant reference to CLI::App (CLI11)
   */
  static const CLI::App &ConstRef();

  // Setters

  /**
   * @brief Add toolchain with a unique name and description
   *
   * @param out Receive the toolchain information through the CLI
   * @param initial Set the default toolchain information as a fallback
   */
  static void AddToolchain(const std::string &name,
                           const std::string &description, ArgToolchain &out,
                           const ArgToolchain &initial = ArgToolchain());

  /**
   * @brief Add Target config commands with a unique name and description
   *
   * @param out Receive the target command information through the CLI
   * @param initial Set the default target command information as a fallback
   *
   * TODO, Update with other options for TargetConfig
   */
  static void AddTarget(const std::string &name, const std::string &description,
                        ArgTarget &out, const ArgTarget &initial = ArgTarget());

  // Getters
  static bool Clean();
  static env::LogLevel GetLogLevel();

  static const fs::path &GetProjectRootDir();
  static const fs::path &GetProjectBuildDir();

private:
  static void RootArgs();
};

} // namespace buildcc

#endif
