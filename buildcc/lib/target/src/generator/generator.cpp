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

void Generator::AddGenInfo(const std::string &name,
                           const internal::fs_unordered_set &inputs,
                           const internal::fs_unordered_set &outputs,
                           const std::vector<std::string> &commands,
                           bool parallel) {
  env::assert_fatal(current_info_.find(name) == current_info_.end(),
                    fmt::format("'{}' information already registered", name));
  current_info_.emplace(name, internal::GenInfo::CreateUserGenInfo(
                                  name, inputs, outputs, commands, parallel));
}

void Generator::AddRegenerateCb(const std::function<bool(void)> &cb) {
  if (cb) {
    regenerate_cb_ = cb;
  }
}

void Generator::AddPregenerateCb(const std::function<void(void)> &cb) {
  pregenerate_cb_ = cb;
}

void Generator::AddPostgenerateCb(const std::function<void(void)> &cb) {
  postgenerate_cb_ = cb;
}

void Generator::Build() { GenerateTask(); }

// PRIVATE

void Generator::Convert() {
  for (auto &ci : current_info_) {
    for (const auto &user_i : ci.second.inputs.user) {
      ci.second.inputs.internal.emplace(
          internal::Path::CreateExistingPath(user_i));
    }
  }
}

std::vector<const internal::GenInfo *> Generator::BuildGenerate() {
  const bool loaded = loader_.Load();
  std::vector<const internal::GenInfo *> generated_files;
  bool build = false;
  if (!loaded) {
    std::for_each(
        current_info_.cbegin(), current_info_.cend(),
        [&](const auto &p) { generated_files.push_back(&(p.second)); });
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

  // Previous Info has more items than Current Info
  for (const auto &pi : previous_info) {
    if (current_info_.find(pi.first) == current_info_.end()) {
      build = true;
      break;
    }
  }

  for (const auto &ci : current_info_) {
    if (previous_info.find(ci.first) == previous_info.end()) {
      // This means that current_info has more items than
      // previous_info
      dirty_ = true;
    } else {
      if (regenerate_cb_()) {
        dirty_ = true;
      }
      const internal::GenInfo &loaded_geninfo = previous_info.at(ci.first);
      RecheckPaths(
          loaded_geninfo.inputs.internal, ci.second.inputs.internal,
          [&]() { InputRemoved(); }, [&]() { InputAdded(); },
          [&]() { InputUpdated(); });
      RecheckChanged(loaded_geninfo.outputs, ci.second.outputs,
                     [&]() { OutputChanged(); });
      RecheckChanged(loaded_geninfo.commands, ci.second.commands,
                     [&]() { CommandChanged(); });
    }

    if (dirty_) {
      generated_files.push_back(&(ci.second));
      build = true;
    }
    dirty_ = false;
  }
  return build;
}

} // namespace buildcc::base
