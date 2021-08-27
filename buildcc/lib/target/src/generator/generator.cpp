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

#include <algorithm>

#include "env/assert_fatal.h"

#include "command/command.h"

namespace buildcc::base {

void Generator::AddGenInfo(const internal::GenInfo &info) {
  env::assert_fatal(
      current_info_.find(info.name) == current_info_.end(),
      fmt::format("'{}' information already registered", info.name));
  current_info_[info.name] = info;
}

void Generator::Build() {
  // TODO, Handle parallel case
  build_task_ = tf_.emplace([&](tf::Subflow &subflow) {
    const auto &generated_files = BuildGenerate();
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
  });
  build_task_.name(fmt::format("BuildTask:{}", name_));
}

// PRIVATE

std::vector<const internal::GenInfo *> Generator::BuildGenerate() {
  const bool loaded = loader_.Load();

  std::vector<const internal::GenInfo *> generated_files;
  if (!loaded) {
    std::for_each(current_info_.cbegin(), current_info_.cend(),
                  [&](const std::pair<std::string, internal::GenInfo> &p) {
                    generated_files.push_back(&(p.second));
                  });
  } else {
    std::for_each(
        current_info_.cbegin(), current_info_.cend(),
        [&](const std::pair<std::string, internal::GenInfo> &p) {
          // Recheck inputs for change in timestamp
          RecheckPaths(
              p.second.inputs, current_info_[p.first].inputs, []() {}, []() {},
              []() {});

          // Recheck presence of outputs
          RecheckChanged(p.second.outputs, current_info_[p.first].outputs);

          // Recheck change in commands
          RecheckChanged(p.second.commands, current_info_[p.first].commands);

          if (dirty_) {
            generated_files.push_back(&(p.second));
          }
          dirty_ = false;
        });
  }

  if (!generated_files.empty()) {
    dirty_ = true;
  }

  return generated_files;
}

} // namespace buildcc::base
