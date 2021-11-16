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

#include "target/target.h"

#include "target/util.h"

#include "env/assert_fatal.h"

#include "fmt/format.h"

namespace buildcc::base {

// Public
void Target::AddSourceAbsolute(const fs::path &absolute_source) {
  state_.ExpectsUnlock();
  env::assert_fatal(config_.IsValidSource(absolute_source),
                    fmt::format("{} does not have a valid source extension",
                                absolute_source));
  storer_.current_source_files.user.emplace(
      fs::path(absolute_source).make_preferred());
}

void Target::GlobSourcesAbsolute(const fs::path &absolute_source_dir) {
  for (const auto &p : fs::directory_iterator(absolute_source_dir)) {
    if (config_.IsValidSource(p.path())) {
      AddSourceAbsolute(p.path());
    }
  }
}

void Target::AddSource(const fs::path &relative_source,
                       const std::filesystem::path &relative_to_target_path) {
  env::log_trace(name_, __FUNCTION__);
  // Compute the absolute source path
  fs::path absolute_source =
      GetTargetRootDir() / relative_to_target_path / relative_source;
  AddSourceAbsolute(absolute_source);
}

void Target::GlobSources(const fs::path &relative_to_target_path) {
  env::log_trace(name_, __FUNCTION__);
  fs::path absolute_input_path = GetTargetRootDir() / relative_to_target_path;
  for (const auto &p : fs::directory_iterator(absolute_input_path)) {
    if (config_.IsValidSource(p.path())) {
      AddSourceAbsolute(p.path());
    }
  }
}

} // namespace buildcc::base
