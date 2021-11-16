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

namespace buildcc::base {

void Target::AddPchAbsolute(const fs::path &absolute_filepath) {
  state_.ExpectsUnlock();
  env::assert_fatal(config_.IsValidHeader(absolute_filepath),
                    fmt::format("{} does not have a valid header extension",
                                absolute_filepath));

  const fs::path absolute_pch = fs::path(absolute_filepath).make_preferred();
  storer_.current_pch_files.user.insert(absolute_pch);
}

void Target::AddPch(const fs::path &relative_filename,
                    const fs::path &relative_to_target_path) {
  env::log_trace(name_, __FUNCTION__);

  // Compute the absolute source path
  fs::path absolute_pch =
      GetTargetRootDir() / relative_to_target_path / relative_filename;

  AddPchAbsolute(absolute_pch);
}

} // namespace buildcc::base
