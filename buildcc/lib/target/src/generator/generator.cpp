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

#include "flatbuffers/flexbuffers.h"

#include "env/assert_fatal.h"

namespace {

class GeneratorBlobHandler : public buildcc::CustomBlobHandler {
public:
  GeneratorBlobHandler(const std::vector<std::string> &commands)
      : commands_(commands) {}

private:
  std::vector<std::string> commands_;

  bool Verify(const std::vector<uint8_t> &serialized_data) const override {
    auto flex_ref = flexbuffers::GetRoot(serialized_data);
    if (!flex_ref.IsVector()) {
      return false;
    }
    auto flex_vect = flex_ref.AsVector();
    for (size_t i = 0; i < flex_vect.size(); i++) {
      if (!flex_vect[i].IsString()) {
        return false;
      }
    }
    return true;
  }

  bool IsEqual(const std::vector<uint8_t> &previous,
               const std::vector<uint8_t> &current) const override {
    return Deserialize(previous) == Deserialize(current);
  }

  std::vector<uint8_t> Serialize() const override {
    flexbuffers::Builder builder;
    builder.Vector([&]() {
      for (const auto &c : commands_) {
        builder.Add(c);
      }
    });
    builder.Finish();
    return builder.GetBuffer();
  }

  // serialized_data has already been verified
  static std::vector<std::string>
  Deserialize(const std::vector<uint8_t> &serialized_data) {
    std::vector<std::string> deserialized;
    auto flex_vect = flexbuffers::GetRoot(serialized_data).AsVector();
    for (size_t i = 0; i < flex_vect.size(); i++) {
      deserialized.emplace_back(flex_vect[i].AsString().str());
    }
    return deserialized;
  }
};

} // namespace

namespace buildcc {

void Generator::AddInput(const std::string &absolute_input_pattern,
                         const char *identifier) {
  std::string absolute_input_string = Construct(absolute_input_pattern);
  const auto absolute_input_path =
      internal::Path::CreateNewPath(absolute_input_string);
  inputs_.insert(absolute_input_path.GetPathname());

  if (identifier != nullptr) {
    AddDefaultArgument(identifier, absolute_input_path.GetPathAsString());
  }
}

void Generator::AddOutput(const std::string &absolute_output_pattern,
                          const char *identifier) {
  std::string absolute_output_string = Construct(absolute_output_pattern);
  const auto absolute_output_path =
      internal::Path::CreateNewPath(absolute_output_string);
  outputs_.insert(absolute_output_path.GetPathname());

  if (identifier != nullptr) {
    AddDefaultArgument(identifier, absolute_output_path.GetPathAsString());
  }
}

void Generator::AddCommand(
    const std::string &command_pattern,
    const std::unordered_map<const char *, std::string> &arguments) {
  std::string constructed_command = Construct(command_pattern, arguments);
  commands_.emplace_back(std::move(constructed_command));
}

void Generator::Build() {
  AddGenInfo(
      "Generate", inputs_, outputs_,
      [&](const CustomGeneratorContext &ctx) -> bool {
        (void)ctx;
        bool success = true;
        for (const auto &c : commands_) {
          bool executed = env::Command::Execute(c);
          if (!executed) {
            success = false;
            env::log_critical(__FUNCTION__,
                              fmt::format("Failed to run command {}", c));
            break;
          }
        }
        return success;
      },
      std::make_shared<GeneratorBlobHandler>(commands_));
  this->CustomGenerator::Build();
}

// PRIVATE

} // namespace buildcc
