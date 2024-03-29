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

#include "buildexe/toolchain_setup.h"

namespace {

constexpr const char *const kTag = "BuildExe";

}

namespace buildcc {

void host_toolchain_verify(const BaseToolchain &toolchain) {
  env::log_info(kTag, "*** Starting Toolchain verification ***");

  fs::path file = Project::GetBuildDir() / "verify_host_toolchain" /
                  "verify_host_toolchain.cpp";
  fs::create_directories(file.parent_path());
  std::string file_data = R"(// Generated by BuildExe
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

int main() {
  std::cout << "********************" << std::endl;
  std::cout << "Verifying host toolchain" << std::endl;
  std::cout << "Current Path: " << fs::current_path() << std::endl;
  std::cout << "********************" << std::endl;
  return 0;
})";
  env::save_file(path_as_string(file).c_str(), file_data, false);

  ExecutableTarget_generic target(
      "verify", toolchain, TargetEnv(file.parent_path(), file.parent_path()));

  target.AddSource(file);
  switch (toolchain.GetId()) {
  case ToolchainId::Gcc:
  case ToolchainId::MinGW:
    target.AddCppCompileFlag("-std=c++17");
    break;
  case ToolchainId::Msvc:
    target.AddCppCompileFlag("/std:c++17");
    break;
  default:
    env::assert_fatal<false>("Invalid Compiler Id");
  }
  target.Build();

  // Build
  tf::Executor executor;
  executor.run(target.GetTaskflow());
  executor.wait_for_all();
  env::assert_fatal(env::get_task_state() == env::TaskState::SUCCESS,
                    "Input toolchain could not compile host program. "
                    "Requires HOST toolchain");

  // Run
  bool execute = env::Command::Execute(fmt::format(
      "{executable}", fmt::arg("executable", target.GetTargetPath().string())));
  env::assert_fatal(execute, "Could not execute verification target");

  env::log_info(kTag, "*** Toolchain verification done ***");
}

} // namespace buildcc
