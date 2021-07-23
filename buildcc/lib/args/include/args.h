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

#ifndef ARGS_INCLUDE_ARGS_H_
#define ARGS_INCLUDE_ARGS_H_

#include <filesystem>

// Third Party
#include "CLI/CLI.hpp"

// BuildCC
#include "env/logging.h"

#include "toolchain.h"

namespace fs = std::filesystem;

namespace buildcc {

class Args {
public:
  struct ToolchainState {
    bool build{false};
    bool test{false};
  };

  struct ToolchainArg {
    ToolchainArg(){};

    base::Toolchain ConstructToolchain() {
      base::Toolchain toolchain(id, name, asm_compiler, c_compiler,
                                cpp_compiler, archiver, linker);
      return toolchain;
    }

    ToolchainState state;
    base::Toolchain::Id id{base::Toolchain::Id::Undefined};
    std::string name{""};
    std::string asm_compiler{""};
    std::string c_compiler{""};
    std::string cpp_compiler{""};
    std::string archiver{""};
    std::string linker{""};
  };

  struct TargetArg {
    TargetArg(){};

    std::string compile_command{""};
    std::string link_command{""};
  };

public:
  Args() { Initialize(); }
  Args(const Args &) = delete;

  void Parse(int argc, char **argv);

  // TODO, Check if these are necessary
  CLI::App &Ref() { return app_; }
  const CLI::App &ConstRef() const { return app_; }

  // Setters
  void AddCustomToolchain(const std::string &name,
                          const std::string &description, ToolchainArg &out,
                          const ToolchainArg &initial = ToolchainArg());
  void AddCustomTarget(const std::string &name, const std::string &description,
                       TargetArg &out, const TargetArg &initial = TargetArg());

  // Getters
  bool Clean() const { return clean_; }
  env::LogLevel GetLogLevel() const { return loglevel_; }

  const fs::path &GetProjectRootDir() const { return project_root_dir_; }
  const fs::path &GetProjectBuildDir() const { return project_build_dir_; }

  // Arg supported toolchains
  // TODO, Add more as needed
  const ToolchainState &GetGccState() const { return gcc_state_; }
  const ToolchainState &GetMsvcState() const { return msvc_state_; }

private:
  void Initialize();

  void RootArgs();
  void CommonToolchainArgs();
  void CommonTargetArgs();

  CLI::App *AddToolchain(const std::string &name,
                         const std::string &description,
                         const std::string &group,
                         ToolchainState &toolchain_state);

private:
  CLI::App app_{"BuildCC buildsystem"};

  bool clean_{false};
  env::LogLevel loglevel_{env::LogLevel::Info};
  const std::map<std::string, env::LogLevel> loglevel_map_{
      {"Trace", env::LogLevel::Trace},
      {"Debug", env::LogLevel::Debug},
      {"Info", env::LogLevel::Info},
      {"Warning", env::LogLevel::Warning},
      {"Critical", env::LogLevel::Critical},
  };

  const std::map<std::string, base::Toolchain::Id> toolchain_id_map_{
      {"Gcc", base::Toolchain::Id::Gcc},
      {"Msvc", base::Toolchain::Id::Msvc},
      {"Clang", base::Toolchain::Id::Clang},
      {"Custom", base::Toolchain::Id::Custom},
      {"Undefined", base::Toolchain::Id::Undefined},
  };

  // directory
  fs::path project_root_dir_{""};
  fs::path project_build_dir_{"_internal"};

  ToolchainState gcc_state_{false, false};
  ToolchainState msvc_state_{false, false};

  // Internal
  CLI::App *toolchain_{nullptr};
  CLI::App *target_{nullptr};
};

} // namespace buildcc

#endif
