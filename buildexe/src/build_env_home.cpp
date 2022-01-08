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

#include "buildexe/build_env_home.h"

namespace buildcc {

fs::path BuildccHome::buildcc_home_{""};
fs::path BuildccHome::buildcc_base_{""};
fs::path BuildccHome::buildcc_libs_{""};
fs::path BuildccHome::buildcc_extensions_{""};
bool BuildccHome::initialized_{false};

void BuildccHome::Init() {
  env::assert_fatal(!initialized_, "BuildccHome is already initialized");

  const char *buildcc_home = getenv("BUILDCC_HOME");
  env::assert_fatal(buildcc_home != nullptr,
                    "BUILDCC_HOME environment variable not defined");

  // NOTE, Verify BUILDCC_HOME
  buildcc_home_ = fs::path(buildcc_home);
  buildcc_base_ = buildcc_home_ / "buildcc";
  buildcc_libs_ = buildcc_home_ / "libs";
  buildcc_extensions_ = buildcc_home_ / "extensions";

  env::assert_fatal(fs::exists(buildcc_home_), "{BUILDCC_HOME} path not found");
  env::assert_fatal(fs::exists(buildcc_base_),
                    "{BUILDCC_HOME}/buildcc path not found");
  env::assert_fatal(fs::exists(buildcc_libs_),
                    "{BUILDCC_HOME}/libs path not found");
  env::assert_fatal(fs::exists(buildcc_extensions_),
                    "{BUILDCC_HOME}/extensions path not found");

  initialized_ = true;
}

} // namespace buildcc
