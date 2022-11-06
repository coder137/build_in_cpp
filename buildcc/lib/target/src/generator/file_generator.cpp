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

class FileGeneratorBlobHandler : public buildcc::CustomBlobHandler {
public:
  explicit FileGeneratorBlobHandler(const std::vector<std::string> &commands)
      : commands_(commands) {}

  // serialized_data has already been verified
  static std::vector<std::string>
  Deserialize(const std::vector<uint8_t> &serialized_data) {
    json j = json::from_msgpack(serialized_data, true, false);
    std::vector<std::string> deserialized;
    j.get_to(deserialized);
    return deserialized;
  }

private:
  const std::vector<std::string> &commands_;

  bool Verify(const std::vector<uint8_t> &serialized_data) const override {
    json j = json::from_msgpack(serialized_data, true, false);
    return !j.is_discarded();
  }

  bool IsEqual(const std::vector<uint8_t> &previous,
               const std::vector<uint8_t> &current) const override {
    return Deserialize(previous) == Deserialize(current);
  }

  std::vector<uint8_t> Serialize() const override {
    json j = commands_;
    return json::to_msgpack(j);
  }
};

bool FileGeneratorGenerateCb(const buildcc::CustomGeneratorContext &ctx) {
  (void)ctx;
  bool success = true;
  std::vector<std::string> commands =
      FileGeneratorBlobHandler::Deserialize(ctx.userblob);
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
  AddIdInfo("Generate", inputs_, outputs_, FileGeneratorGenerateCb,
            std::make_shared<FileGeneratorBlobHandler>(commands_));
  this->CustomGenerator::Build();
}

// PRIVATE

} // namespace buildcc
