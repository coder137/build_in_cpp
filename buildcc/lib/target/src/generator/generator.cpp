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

void Generator::AddGenInfo(const UserGenInfo &info) {
  env::assert_fatal(
      user_info_.find(info.name) == user_info_.end(),
      fmt::format("'{}' information already registered", info.name));
  user_info_.insert(std::make_pair(info.name, info));
}

void Generator::Build() { GenerateTask(); }

// PRIVATE

void Generator::Convert() {
  for (const auto &user_info : user_info_) {
    internal::path_unordered_set current_inputs;
    for (const auto &user_inputs : user_info.second.inputs) {
      current_inputs.insert(internal::Path::CreateExistingPath(user_inputs));
    }
    current_info_.insert(std::make_pair(
        user_info.first,
        internal::GenInfo(user_info.second.name, current_inputs,
                          user_info.second.outputs, user_info.second.commands,
                          user_info.second.parallel)));
  }
}

std::vector<const internal::GenInfo *> Generator::BuildGenerate() {
  const bool loaded = loader_.Load();

  std::vector<const internal::GenInfo *> generated_files;
  bool build = false;
  if (!loaded) {
    for (const auto &ci : current_info_) {
      generated_files.push_back(&(ci.second));
    }
    build = true;
  } else {
    build = Regenerate(generated_files);
  }

  dirty_ = build;
  return generated_files;
}

bool Generator::Regenerate(
    std::vector<const internal::GenInfo *> &generated_files) {
  bool build = false;
  const auto &previous_info = loader_.GetLoadedInfo();

  for (const auto &p : current_info_) {
    try {
      const internal::GenInfo &loaded_geninfo = previous_info.at(p.first);
      RecheckPaths(
          loaded_geninfo.inputs, p.second.inputs, [&]() { InputRemoved(); },
          [&]() { InputAdded(); }, [&]() { InputUpdated(); });
      RecheckChanged(loaded_geninfo.outputs, p.second.outputs,
                     [&]() { OutputChanged(); });
      RecheckChanged(loaded_geninfo.commands, p.second.commands,
                     [&]() { CommandChanged(); });

      if (dirty_) {
        generated_files.push_back(&(p.second));
        build = true;
      }
      dirty_ = false;
    } catch (const std::out_of_range &e) {
      // This means that current_info has more items than
      // previous_info
      build = true;
    }
  }
  return build;
}

} // namespace buildcc::base
