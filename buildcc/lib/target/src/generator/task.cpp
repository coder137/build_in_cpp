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
  build_task_ = tf_.emplace([&](tf::Subflow &subflow) {
    pregenerate_cb_();
    Convert();
    const auto generated_files = BuildGenerate();
    if (!dirty_) {
      return;
    }

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
            .emplace([&]() {
              for (const auto &command : info->commands) {
                bool success = Command::Execute(command);
                env::assert_fatal(success, fmt::format("{} failed", command));
              }
            })
            .name(info->name);
      }
    }
    Store();
    postgenerate_cb_();
  });
  build_task_.name(fmt::format("BuildTask:{}", name_));
}

} // namespace buildcc::base
