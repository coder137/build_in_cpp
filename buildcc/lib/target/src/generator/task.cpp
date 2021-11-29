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
constexpr const char *const kStartGeneratorTaskName = "Start Generator";
constexpr const char *const kEndGeneratorTaskName = "End Generator";

constexpr const char *const kCommandTaskName = "Command";
constexpr const char *const kGenerateTaskName = "Generate";

} // namespace

namespace buildcc::base {

void Generator::GenerateTask() {
  tf::Task start_task = tf_.emplace([this]() {
    switch (env::get_task_state()) {
    case env::TaskState::SUCCESS:
      break;
    default:
      task_state_ = env::TaskState::FAILURE;
      break;
    }
    return static_cast<int>(task_state_);
  });
  start_task.name(kStartGeneratorTaskName);

  tf::Task generate_task = tf_.emplace([&](tf::Subflow &subflow) {
    try {
      Convert();
      BuildGenerate();
    } catch (...) {
      task_state_ = env::TaskState::FAILURE;
    }

    auto run_command = [this](const std::string &command) {
      try {
        bool success = Command::Execute(command);
        env::assert_throw(success, fmt::format("{} failed", command));
      } catch (...) {
        task_state_ = env::TaskState::FAILURE;
      }
    };

    tf::Task command_task;
    if (dirty_ && (task_state_ == env::TaskState::SUCCESS)) {
      if (parallel_) {
        command_task = subflow.for_each(current_commands_.cbegin(),
                                        current_commands_.cend(), run_command);
      } else {
        command_task = subflow.emplace([&]() {
          for (const auto &command : current_commands_) {
            run_command(command);
          }
        });
      }
    } else {
      command_task = subflow.placeholder();
    }
    command_task.name(kCommandTaskName);

    // Graph Generation
    for (const auto &i : current_input_files_.user) {
      std::string name =
          fmt::format("{}", i.lexically_relative(env::get_project_root_dir()));
      tf::Task task = subflow.placeholder().name(name);
      task.precede(command_task);
    }

    for (const auto &o : current_output_files_) {
      std::string name =
          fmt::format("{}", o.lexically_relative(env::get_project_root_dir()));
      tf::Task task = subflow.placeholder().name(name);
      task.succeed(command_task);
    }
  });
  generate_task.name(kGenerateTaskName);

  tf::Task end_task = tf_.emplace([this]() {
    // task_state_ != env::TaskState::SUCCESS
    // We do not need to Store, leave the serialized store with the previous
    // values

    // NOTE, Only store if the above state is marked dirty_ AND task_state_ ==
    // SUCCESS
    if (dirty_ && (task_state_ == env::TaskState::SUCCESS)) {
      try {
        env::assert_throw(Store(), fmt::format("Store failed for {}", name_));
      } catch (...) {
        task_state_ = env::TaskState::FAILURE;
      }
    }

    // Update Env task state when NOT SUCCESS only
    if (task_state_ != env::TaskState::SUCCESS) {
      env::set_task_state(task_state_);
    }
  });
  end_task.name(kEndGeneratorTaskName);

  // Dependencies
  start_task.precede(generate_task, end_task);
  generate_task.precede(end_task);
}

} // namespace buildcc::base
