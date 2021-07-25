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

#include "env/assert_fatal.h"
#include "env/logging.h"

#include "target/util.h"

#include "fmt/format.h"

namespace buildcc::base {

void Target::AddHeaderAbsolute(const fs::path &absolute_filepath) {
  env::assert_fatal(IsValidHeader(absolute_filepath),
                    fmt::format("{} does not have a valid header extension",
                                absolute_filepath.string()));
  internal::add_path(absolute_filepath, current_header_files_);
}

void Target::AddHeader(const std::string &relative_filename,
                       const fs::path &relative_to_target_path) {
  env::log_trace(name_, __FUNCTION__);

  // Check Source
  fs::path absolute_filepath =
      target_root_source_dir_ / relative_to_target_path / relative_filename;
  AddHeaderAbsolute(absolute_filepath);
}

void Target::AddHeader(const std::string &relative_filename) {
  AddHeader(relative_filename, "");
}

void Target::GlobHeaders(const fs::path &relative_to_target_path) {
  env::log_trace(name_, __FUNCTION__);

  fs::path absolute_path = target_root_source_dir_ / relative_to_target_path;
  GlobHeadersAbsolute(absolute_path);
}

void Target::GlobHeadersAbsolute(const fs::path &absolute_path) {
  for (const auto &p : fs::directory_iterator(absolute_path)) {
    if (IsValidHeader(p.path())) {
      env::log_trace(name_, fmt::format("Added header {}", p.path().string()));
      AddHeaderAbsolute(p.path());
    }
  }
}

// Public
void Target::AddIncludeDir(const fs::path &relative_include_dir,
                           bool glob_headers) {
  env::log_trace(name_, __FUNCTION__);

  const fs::path absolute_include_dir =
      (target_root_source_dir_ / relative_include_dir);
  AddIncludeDirAbsolute(absolute_include_dir, glob_headers);
}

void Target::AddIncludeDirAbsolute(const fs::path &absolute_include_dir,
                                   bool glob_headers) {
  current_include_dirs_.insert(absolute_include_dir);

  if (glob_headers) {
    GlobHeadersAbsolute(absolute_include_dir);
  }
}

} // namespace buildcc::base
