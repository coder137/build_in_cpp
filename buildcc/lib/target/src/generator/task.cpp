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

namespace buildcc::base {

void Generator::GenerateTask() {

  tf::Task pregenerate_task = tf_.emplace([this]() {
    pregenerate_cb_();
    Convert();
  });

  tf::Task postgenerate_task = tf_.emplace([this]() {
    if (dirty_) {
      Store();
      postgenerate_cb_();
    }
  });

  build_task_ = tf_.emplace([&](tf::Subflow &subflow) {
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

  build_task_.succeed(pregenerate_task);
  build_task_.precede(postgenerate_task);

  pregenerate_task.name("PreGenerate");
  postgenerate_task.name("PostGenerate");
  build_task_.name(name_);
  tf_.name(name_);
}

} // namespace buildcc::base
