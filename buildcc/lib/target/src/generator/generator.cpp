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

#include "target/generator.h"

#include <algorithm>

#include "env/assert_fatal.h"

namespace buildcc {

void Generator::AddDefaultArguments(
    const std::unordered_map<std::string, std::string> &arguments) {
  command_.AddDefaultArguments(arguments);
}

void Generator::AddInput(const std::string &absolute_input_pattern,
                         const char *identifier) {
  std::string absolute_input_string =
      command_.Construct(absolute_input_pattern);
  const auto absolute_input_path =
      internal::Path::CreateNewPath(absolute_input_string);
  user_.inputs.insert(absolute_input_path.GetPathname());

  if (identifier != nullptr) {
    command_.AddDefaultArgument(identifier,
                                absolute_input_path.GetPathAsString());
  }
}

void Generator::AddOutput(const std::string &absolute_output_pattern,
                          const char *identifier) {
  std::string absolute_output_string =
      command_.Construct(absolute_output_pattern);
  const auto absolute_output_path =
      internal::Path::CreateNewPath(absolute_output_string);
  user_.outputs.insert(absolute_output_path.GetPathname());

  if (identifier != nullptr) {
    command_.AddDefaultArgument(identifier,
                                absolute_output_path.GetPathAsString());
  }
}

void Generator::AddCommand(
    const std::string &command_pattern,
    const std::unordered_map<const char *, std::string> &arguments) {
  std::string constructed_command =
      command_.Construct(command_pattern, arguments);
  user_.commands.emplace_back(std::move(constructed_command));
}

void Generator::Build() {
  (void)serialization_.LoadFromFile();

  GenerateTask();
}

const std::string &
Generator::GetValueByIdentifier(const std::string &file_identifier) const {
  return command_.GetDefaultValueByKey(file_identifier);
}

// PRIVATE

void Generator::Initialize() {
  // Checks
  env::assert_fatal(
      env::is_init(),
      "Environment is not initialized. Use the buildcc::env::init API");

  //
  fs::create_directories(generator_build_dir_);
  command_.AddDefaultArguments({
      {"gen_root_dir", path_as_string(generator_root_dir_)},
      {"gen_build_dir", path_as_string(generator_build_dir_)},
  });

  //
  unique_id_ = name_;
  tf_.name(name_);
}

void Generator::Convert() {
  user_.internal_inputs = internal::path_schema_convert(
      user_.inputs, internal::Path::CreateExistingPath);
}

void Generator::BuildGenerate() {
  if (!serialization_.IsLoaded()) {
    dirty_ = true;
  } else {
    RecheckPaths(
        serialization_.GetLoad().internal_inputs, user_.internal_inputs,
        [&]() { InputRemoved(); }, [&]() { InputAdded(); },
        [&]() { InputUpdated(); });
    RecheckChanged(serialization_.GetLoad().outputs, user_.outputs,
                   [&]() { OutputChanged(); });
    RecheckChanged(serialization_.GetLoad().commands, user_.commands,
                   [&]() { CommandChanged(); });
  }
}

} // namespace buildcc
