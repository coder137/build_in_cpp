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

#include "target/generator.h"

#include "command/command.h"

namespace {

constexpr const char *const kPreGenerateTaskName = "PreGenerate";
constexpr const char *const kGenerateTaskName = "Generate";
constexpr const char *const kPostGenerateTaskName = "PostGenerate";

} // namespace

namespace buildcc::base {

void Generator::GenerateTask() {

  tf::Task pregenerate_task = tf_.emplace([this]() {
    Convert();
    BuildGenerate();
    return dirty_;
  });
  pregenerate_task.name(kPreGenerateTaskName);

  tf::Task postgenerate_task = tf_.emplace([this]() {
    if (dirty_) {
      Store();
    }
  });
  postgenerate_task.name(kPostGenerateTaskName);

  tf::Task generate_task;
  if (parallel_) {
    generate_task = tf_.for_each(
        current_commands_.cbegin(), current_commands_.cend(),
        [](const std::string &command) {
          bool success = Command::Execute(command);
          env::assert_fatal(success, fmt::format("{} failed", command));
        });
  } else {
    generate_task = tf_.emplace([&]() {
      for (const auto &command : current_commands_) {
        bool success = Command::Execute(command);
        env::assert_fatal(success, fmt::format("{} failed", command));
      }
    });
  }
  generate_task.name(kGenerateTaskName);

  pregenerate_task.precede(postgenerate_task, generate_task);
  generate_task.precede(postgenerate_task);
}

} // namespace buildcc::base
