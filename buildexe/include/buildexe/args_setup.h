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

#ifndef BUILDEXE_ARGS_SETUP_H_
#define BUILDEXE_ARGS_SETUP_H_

#include "buildcc.h"

namespace buildcc {

enum class BuildExeMode {
  Immediate,
  Script,
};

struct ArgTargetInfo : public ArgCustom {
  void Add(CLI::App &app) override;

  std::string name;
  TargetType type;
  fs::path relative_to_root;
};

struct ArgTargetInputs : public ArgCustom {
  void Add(CLI::App &app) override;

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

struct ArgScriptInfo : public ArgCustom {
  void Add(CLI::App &app) override;

  std::vector<std::string> configs;
};

struct LibInfo {
  std::string lib_name;
  std::string absolute_lib_path;
};

struct ArgLibsInfo : public ArgCustom {
  void Add(CLI::App &app) override;

  std::vector<LibInfo> libs_info;
  std::vector<fs::path> lib_build_files;
};

class BuildExeArgs {
public:
  void Setup(int argc, char **argv);

  // Getters
  ArgToolchain &GetHostToolchainArg() { return host_toolchain_arg_; }
  const ArgTargetInfo &GetTargetInfo() const { return out_targetinfo_; }
  const ArgTargetInputs &GetTargetInputs() const { return out_targetinputs_; }
  const ArgScriptInfo &GetScriptInfo() const { return out_scriptinfo_; }
  BuildExeMode GetBuildMode() const { return out_mode_; }

  const std::vector<LibInfo> &GetLibsInfo() const {
    return out_libsinfo_.libs_info;
  }
  const std::vector<fs::path> &GetLibBuildFiles() const {
    return out_libsinfo_.lib_build_files;
  }

private:
  void SetupBuildMode(CLI::App &app);

private:
  ArgToolchain host_toolchain_arg_;
  ArgTargetInfo out_targetinfo_;
  ArgTargetInputs out_targetinputs_;
  ArgScriptInfo out_scriptinfo_;
  ArgLibsInfo out_libsinfo_;
  BuildExeMode out_mode_;
};

} // namespace buildcc

#endif
