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

std::string GetCompilerVersion(const fs::path &absolute_path,
                               const buildcc::base::Toolchain &toolchain) {
  buildcc::Command command;
  command.AddDefaultArgument(
      "compiler",
      (absolute_path / toolchain.GetCppCompiler()).make_preferred().string());
  std::string compiler_version;
  std::vector<std::string> stdout_data;

  auto gcc_get_compiler_version = [&]() {
    bool executed = buildcc::Command::Execute(
        command.Construct("{compiler} -dumpversion"), {}, &stdout_data);
    buildcc::env::assert_fatal(
        executed, "GetCompilerVersion command not executed successfully");
    compiler_version = stdout_data.at(0);
  };

  auto msvc_get_compiler_version = [&]() {
    // TODO, Read `VSCMD_VER` from ENV Path
  };

  switch (toolchain.GetId()) {
  case buildcc::base::Toolchain::Id::Gcc:
  case buildcc::base::Toolchain::Id::MinGW:
  case buildcc::base::Toolchain::Id::Clang:
    gcc_get_compiler_version();
    break;
  case buildcc::base::Toolchain::Id::Msvc:
    msvc_get_compiler_version();
    break;
  default:
    buildcc::env::assert_fatal<false>(
        "GetCompilerVersion not supported on this Compiler");
    break;
  }
  return compiler_version;
}

std::string GetCompilerArchitecture(const fs::path &absolute_path,
                                    const buildcc::base::Toolchain &toolchain) {
  buildcc::Command command;
  command.AddDefaultArgument(
      "compiler",
      (absolute_path / toolchain.GetCppCompiler()).make_preferred().string());
  std::string target_arch;
  std::vector<std::string> stdout_data;

  auto gcc_get_target_arch = [&]() {
    bool executed = buildcc::Command::Execute(
        command.Construct("{compiler} -dumpmachine"), {}, &stdout_data);
    buildcc::env::assert_fatal(
        executed, "GetCompilerArchitecture command not executed successfully");
    target_arch = stdout_data.at(0);
  };

  auto msvc_get_target_arch = [&]() {
    // TODO, Read `VSCMD_ARG_HOST_ARCH` from env path
    // TODO, Read `VSCMD_ARG_TGT_ARCH` from env path
    // TODO, Concat them!
  };

  switch (toolchain.GetId()) {
  case buildcc::base::Toolchain::Id::Gcc:
  case buildcc::base::Toolchain::Id::MinGW:
  case buildcc::base::Toolchain::Id::Clang:
    gcc_get_target_arch();
    break;
  case buildcc::base::Toolchain::Id::Msvc:
    msvc_get_target_arch();
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
  ToolchainMatcher(const buildcc::base::Toolchain &toolchain)
      : toolchain_(toolchain) {}

  void FillWithToolchainFilenames() {
    constexpr const char *os_executable_ext =
        buildcc::env::get_os_executable_extension();
    buildcc::env::assert_fatal<os_executable_ext != nullptr>(
        "OS not supported");

    matcher_.clear();
    matcher_.insert(fmt::format("{}{}", t.GetAsmCompiler(), os_executable_ext));
    matcher_.insert(fmt::format("{}{}", t.GetCCompiler(), os_executable_ext));
    matcher_.insert(fmt::format("{}{}", t.GetCppCompiler(), os_executable_ext));
    matcher_.insert(fmt::format("{}{}", t.GetArchiver(), os_executable_ext));
    matcher_.insert(fmt::format("{}{}", t.GetLinker(), os_executable_ext));
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
