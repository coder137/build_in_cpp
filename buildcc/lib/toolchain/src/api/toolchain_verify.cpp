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

#include "toolchain/api/toolchain_verify.h"

#include <unordered_set>
#include <vector>

#include "toolchain/toolchain.h"

#include "env/assert_fatal.h"
#include "env/host_os.h"
#include "env/host_os_util.h"
#include "env/util.h"

#include "command/command.h"

namespace {
// Constants

// Functions
std::vector<std::string> ParseEnvVarToPaths(const std::string &env_var) {
  const char *path_env = getenv(env_var.c_str());
  buildcc::env::assert_fatal(
      path_env != nullptr,
      fmt::format("Environment variable '{}' not present", env_var));

  constexpr const char *os_env_delim = buildcc::env::get_os_envvar_delim();
  buildcc::env::assert_fatal<os_env_delim != nullptr>("OS not supported");
  std::vector<std::string> paths =
      buildcc::env::split(path_env, os_env_delim[0]);

  return paths;
}

std::string GetGccCompilerVersion(const buildcc::Command &command) {
  std::vector<std::string> stdout_data;
  bool executed = buildcc::Command::Execute(
      command.Construct("{compiler} -dumpversion"), {}, &stdout_data);
  buildcc::env::assert_fatal(
      executed, "GetCompilerVersion command not executed successfully");
  return stdout_data.at(0);
}

std::string GetMsvcCompilerVersion() {
  // Done VSCMD_VER
  const char *vscmd_version = getenv("VSCMD_VER");
  buildcc::env::assert_fatal(
      vscmd_version != nullptr,
      "Setup Visual Studio build tools. Call `vcvarsall.bat {platform}` to "
      "setup your target and host");
  return vscmd_version;
}

std::string GetCompilerVersion(const fs::path &absolute_path,
                               const buildcc::base::Toolchain &toolchain) {
  buildcc::Command command;
  command.AddDefaultArgument(
      "compiler",
      (absolute_path / toolchain.GetCppCompiler()).make_preferred().string());

  std::string compiler_version;
  switch (toolchain.GetId()) {
  case buildcc::base::Toolchain::Id::Gcc:
  case buildcc::base::Toolchain::Id::MinGW:
  case buildcc::base::Toolchain::Id::Clang:
    compiler_version = GetGccCompilerVersion(command);
    break;
  case buildcc::base::Toolchain::Id::Msvc:
    compiler_version = GetMsvcCompilerVersion();
    break;
  default:
    buildcc::env::assert_fatal<false>(
        "GetCompilerVersion not supported on this Compiler");
    break;
  }
  return compiler_version;
}

std::string GetGccTargetArchitecture(const buildcc::Command &command) {
  std::vector<std::string> stdout_data;
  bool executed = buildcc::Command::Execute(
      command.Construct("{compiler} -dumpmachine"), {}, &stdout_data);
  buildcc::env::assert_fatal(
      executed, "GetCompilerArchitecture command not executed successfully");
  return stdout_data.at(0);
}

std::string GetMsvcTargetArchitecture() {
  // DONE, Read `VSCMD_ARG_HOST_ARCH` from env path
  // DONE, Read `VSCMD_ARG_TGT_ARCH` from env path
  const char *vs_host_arch = getenv("VSCMD_ARG_HOST_ARCH");
  const char *vs_target_arch = getenv("VSCMD_ARG_TGT_ARCH");
  buildcc::env::assert_fatal(
      (vs_host_arch != nullptr) && (vs_target_arch != nullptr),
      "Setup Visual Studio build tools. Call `vcvarsall.bat {platform}` to "
      "setup your target and host");

  // DONE, Concat them!
  return fmt::format("{}_{}", vs_host_arch, vs_target_arch);
}

std::string GetCompilerArchitecture(const fs::path &absolute_path,
                                    const buildcc::base::Toolchain &toolchain) {
  buildcc::Command command;
  command.AddDefaultArgument(
      "compiler",
      (absolute_path / toolchain.GetCppCompiler()).make_preferred().string());
  std::string target_arch;
  switch (toolchain.GetId()) {
  case buildcc::base::Toolchain::Id::Gcc:
  case buildcc::base::Toolchain::Id::MinGW:
  case buildcc::base::Toolchain::Id::Clang:
    target_arch = GetGccTargetArchitecture(command);
    break;
  case buildcc::base::Toolchain::Id::Msvc:
    target_arch = GetMsvcTargetArchitecture();
    break;
  default:
    buildcc::env::assert_fatal<false>(
        "GetCompilerArchitecture not supported on this Compiler");
    break;
  }
  return target_arch;
}

class ToolchainMatcher {
public:
  explicit ToolchainMatcher(const buildcc::base::Toolchain &toolchain)
      : toolchain_(toolchain) {}

  void FillWithToolchainFilenames() {
    constexpr const char *os_executable_ext =
        buildcc::env::get_os_executable_extension();
    buildcc::env::assert_fatal<os_executable_ext != nullptr>(
        "OS not supported");

    matcher_.clear();
    matcher_.insert(
        fmt::format("{}{}", toolchain_.GetAsmCompiler(), os_executable_ext));
    matcher_.insert(
        fmt::format("{}{}", toolchain_.GetCCompiler(), os_executable_ext));
    matcher_.insert(
        fmt::format("{}{}", toolchain_.GetCppCompiler(), os_executable_ext));
    matcher_.insert(
        fmt::format("{}{}", toolchain_.GetArchiver(), os_executable_ext));
    matcher_.insert(
        fmt::format("{}{}", toolchain_.GetLinker(), os_executable_ext));
  }

  void Check(const std::string &filename) { matcher_.erase(filename); }
  bool Found() { return matcher_.empty(); }

private:
  const buildcc::base::Toolchain &toolchain_;

  std::unordered_set<std::string> matcher_;
};

} // namespace

namespace buildcc::base {

template <typename T>
std::vector<VerifiedToolchain>
ToolchainVerify<T>::Verify(const VerifyToolchainConfig &config) {
  // TODO, Convert this to fs::path eventually
  std::unordered_set<std::string> absolute_search_paths{
      config.absolute_search_paths.begin(), config.absolute_search_paths.end()};

  // Parse config envs
  for (const std::string &env_var : config.env_vars) {
    std::vector<std::string> paths = ParseEnvVarToPaths(env_var);
    for (const auto &p : paths) {
      absolute_search_paths.insert(p);
    }
  }

  std::vector<VerifiedToolchain> verified_toolchains;
  const T &t = static_cast<const T &>(*this);

  ToolchainMatcher matcher(t);
  matcher.FillWithToolchainFilenames();
  // Iterate over absolute search paths
  // [Verification] Match ALL toolchains PER directory
  for (const std::string &pstr : absolute_search_paths) {
    fs::path p{pstr};
    if (!fs::exists(p)) {
      continue;
    }

    // For each directory, Check if ALL toolchain filenames are found
    for (const auto &dir_iter : fs::directory_iterator(p)) {
      if (!dir_iter.is_regular_file()) {
        continue;
      }
      const auto &filename = dir_iter.path().filename().string();
      matcher.Check(filename);
    }

    // Store verified toolchain path if found
    if (matcher.Found()) {
      env::log_info(__FUNCTION__, fmt::format("Found: {}", p.string()));

      VerifiedToolchain vt;
      vt.path = p;
      vt.compiler_version = GetCompilerVersion(p, t);
      vt.target_arch = GetCompilerArchitecture(p, t);
      verified_toolchains.push_back(vt);
    }

    // Reset
    matcher.FillWithToolchainFilenames();
  }

  return verified_toolchains;
}

template class ToolchainVerify<Toolchain>;

} // namespace buildcc::base
