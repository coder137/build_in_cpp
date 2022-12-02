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

#include "target/file_generator.h"

#include <algorithm>

#include "env/assert_fatal.h"

namespace {

bool FileGeneratorGenerateCb(const buildcc::CustomGeneratorContext &ctx) {
  (void)ctx;
  bool success = true;
  std::vector<std::string> commands =
      buildcc::TypedCustomBlobHandler<std::vector<std::string>>::Deserialize(
          ctx.userblob);
  for (const auto &c : commands) {
    bool executed = buildcc::env::Command::Execute(c);
    if (!executed) {
      success = false;
      buildcc::env::log_critical(__FUNCTION__,
                                 fmt::format("Failed to run command {}", c));
      break;
    }
  }
  return success;
}

} // namespace

namespace buildcc {

void FileGenerator::AddInput(const std::string &absolute_input_pattern) {
  inputs_.emplace(absolute_input_pattern);
}

void FileGenerator::AddOutput(const std::string &absolute_output_pattern) {
  outputs_.emplace(absolute_output_pattern);
}

void FileGenerator::AddCommand(
    const std::string &command_pattern,
    const std::unordered_map<const char *, std::string> &arguments) {
  std::string constructed_command = ParsePattern(command_pattern, arguments);
  commands_.emplace_back(std::move(constructed_command));
}

void FileGenerator::Build() {
  auto file_blob_handler =
      std::make_shared<TypedCustomBlobHandler<std::vector<std::string>>>(
          commands_);
  AddIdInfo("Generate", inputs_, outputs_, FileGeneratorGenerateCb,
            file_blob_handler);
  this->CustomGenerator::Build();
}

// PRIVATE

} // namespace buildcc
