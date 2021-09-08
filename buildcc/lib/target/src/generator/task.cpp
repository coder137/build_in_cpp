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

void Generator::GenerateTask(tf::FlowBuilder &builder) {

  tf::Task pregenerate_task = builder.emplace([this]() {
    pregenerate_cb_();
    Convert();
  });

  // TODO, See if postgenerate_cb_ needs to be inside the dirty_ condition
  tf::Task postgenerate_task = builder.emplace([this]() {
    if (dirty_) {
      Store();
      postgenerate_cb_();
    }
  });

  tf::Task generate_task = builder.emplace([&](tf::Subflow &subflow) {
    std::vector<const internal::GenInfo *> generated_files;
    std::vector<const internal::GenInfo *> dummy_generated_files;
    BuildGenerate(generated_files, dummy_generated_files);

    for (const auto &info : generated_files) {
      if (info->parallel) {
        subflow
            .for_each(info->commands.cbegin(), info->commands.cend(),
                      [](const std::string &command) {
                        bool success = Command::Execute(command);
                        env::assert_fatal(success,
                                          fmt::format("{} failed", command));
                      })
            .name(info->name);
      } else {
        subflow
            .emplace([info]() {
              for (const auto &command : info->commands) {
                bool success = Command::Execute(command);
                env::assert_fatal(success, fmt::format("{} failed", command));
              }
            })
            .name(info->name);
      }
    }

    for (const auto &info : dummy_generated_files) {
      subflow.placeholder().name(info->name);
    }
  });

  generate_task.succeed(pregenerate_task);
  generate_task.precede(postgenerate_task);

  pregenerate_task.name(kPreGenerateTaskName);
  generate_task.name(kGenerateTaskName);
  postgenerate_task.name(kPostGenerateTaskName);
}

} // namespace buildcc::base
