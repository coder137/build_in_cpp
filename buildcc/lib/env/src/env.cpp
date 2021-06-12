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

#include "env.h"

#include "logging.h"

namespace {

fs::path root_dir_{""};
fs::path build_dir_{""};
bool init_ = false;

} // namespace

namespace buildcc::env {

void init(const fs::path &project_root_dir, const fs::path &project_build_dir) {
  // State
  root_dir_ = project_root_dir;
  build_dir_ = project_build_dir;
  root_dir_.make_preferred();
  build_dir_.make_preferred();

  init_ = true;

  // Logging
  set_log_pattern("%^[%l]%$ %v");
  set_log_level(LogLevel::Info);
}

void deinit() {
  root_dir_ = "";
  build_dir_ = "";
  init_ = false;
}

bool is_init(void) { return init_; }
const fs::path &get_project_root_dir() { return root_dir_; }
const fs::path &get_project_build_dir() { return build_dir_; }

} // namespace buildcc::env
