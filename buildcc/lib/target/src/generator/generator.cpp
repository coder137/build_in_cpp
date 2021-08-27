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

namespace buildcc::base {

void Generator::AddGenInfo(const internal::GenInfo &info) {
  env::assert_fatal(
      current_info_.find(info.name) == current_info_.end(),
      fmt::format("'{}' information already registered", info.name));
  current_info_[info.name] = info;
}

void Generator::Build() { GenerateTask(); }

// PRIVATE

std::vector<const internal::GenInfo *> Generator::BuildGenerate() {
  const bool loaded = loader_.Load();

  std::vector<const internal::GenInfo *> generated_files;
  bool build = false;
  if (!loaded) {
    std::for_each(current_info_.cbegin(), current_info_.cend(),
                  [&](const std::pair<std::string, internal::GenInfo> &p) {
                    generated_files.push_back(&(p.second));
                  });
    build = true;
  } else {
    const auto &previous_info = loader_.GetLoadedInfo();
    std::for_each(
        current_info_.cbegin(), current_info_.cend(),
        [&](const std::pair<std::string, internal::GenInfo> &p) {
          try {
            const internal::GenInfo &loaded_geninfo = previous_info.at(p.first);
            RecheckPaths(
                loaded_geninfo.inputs, p.second.inputs, []() {}, []() {},
                []() {});
            RecheckChanged(loaded_geninfo.outputs, p.second.outputs);
            RecheckChanged(loaded_geninfo.commands, p.second.commands);
            if (dirty_) {
              generated_files.push_back(&(p.second));
              build = true;
            }
            dirty_ = false;
          } catch (const std::out_of_range &e) {
            // This means that current_info has more items than previous_info
            build = true;
          }
        });
  }

  dirty_ = build;
  return generated_files;
}

} // namespace buildcc::base
