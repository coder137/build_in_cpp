/*
 * Copyright 2021-2022 Niket Naidu. All rights reserved.
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

#include "env/env.h"
#include "env/logging.h"

namespace buildcc {

void Project::Init(const fs::path &project_root_dir,
                   const fs::path &project_build_dir) {
  // State
  fs::path root_dir = project_root_dir;
  fs::path build_dir = project_build_dir;
  root_dir.make_preferred();
  build_dir.make_preferred();

  GetStaticRootDir() = root_dir;
  GetStaticBuildDir() = build_dir;
  GetStaticInit() = true;

  // Logging
  env::set_log_pattern("%^[%l]%$ %v");
  env::set_log_level(env::LogLevel::Info);
}
void Project::Deinit() {
  GetStaticRootDir() = "";
  GetStaticBuildDir() = "";
  GetStaticInit() = false;
}

bool Project::IsInit() { return GetStaticInit(); }
const fs::path &Project::GetRootDir() { return GetStaticRootDir(); }
const fs::path &Project::GetBuildDir() { return GetStaticBuildDir(); }

bool &Project::GetStaticInit() {
  static bool is_init = false;
  return is_init;
}
fs::path &Project::GetStaticRootDir() {
  static fs::path root_dir = "";
  return root_dir;
}
fs::path &Project::GetStaticBuildDir() {
  static fs::path build_dir = "";
  return build_dir;
}

} // namespace buildcc
