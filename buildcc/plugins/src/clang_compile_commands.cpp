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

#include "plugins/clang_compile_commands.h"

#include <algorithm>

// env
#include "env/assert_fatal.h"
#include "env/util.h"

// third party
#include "fmt/format.h"

namespace {

// clang-format off
constexpr const char *const clang_compile_command_format =
R"({{
  "directory": "{directory}",
  "command": "{command}",
  "file": "{file}"
}})";
// clang-format on

} // namespace

namespace buildcc::plugin {

void ClangCompileCommands::AddTarget(const BaseTarget *target) {
  env::assert_fatal(target != nullptr, "Target should not be NULL");
  targets_.push_back(target);
}

void ClangCompileCommands::Generate() {
  // Early terminate if rebuild is not required
  const bool regenerate =
      std::any_of(targets_.begin(), targets_.end(),
                  [](const BaseTarget *target) { return target->IsBuilt(); });
  if (!regenerate) {
    env::log_trace("ClangCompileCommands", "Generate -> false");
    return;
  }
  env::log_trace("ClangCompileCommands", "Generate -> true");

  std::vector<std::string> compile_command_list;

  for (const auto *t : targets_) {
    const auto &source_files = t->GetSourceFiles();
    for (const auto &source : source_files) {
      // DONE, Get source list name
      // DONE, Get std::vector<std::string> CompileCommand
      // DONE, Get intermediate directory from env
      std::string sf = fmt::format("{}", source);
      const std::string &command = t->GetCompileCommand(source);
      std::string directory = fmt::format("{}", Project::GetBuildDir());

      std::string temp = fmt::format(
          clang_compile_command_format, fmt::arg("directory", directory),
          fmt::arg("command", command), fmt::arg("file", sf));
      compile_command_list.push_back(temp);
    }
  }

  std::string compile_commands =
      fmt::format("[\n{}\n]", fmt::join(compile_command_list, ",\n"));

  // DONE, Convert to json
  // DONE, Save file
  std::filesystem::path file =
      std::filesystem::path(buildcc::Project::GetBuildDir()) /
      "compile_commands.json";
  bool saved =
      env::save_file(path_as_string(file).c_str(), compile_commands, false);
  env::assert_fatal(saved, "Could not save compile_commands.json");
}

} // namespace buildcc::plugin
