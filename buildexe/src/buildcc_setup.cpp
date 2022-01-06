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

#include "buildexe/buildcc_setup.h"

#include "bootstrap/build_buildcc.h"

namespace buildcc {

fs::path get_env_buildcc_home() {
  const char *buildcc_home = getenv("BUILDCC_HOME");
  env::assert_fatal(buildcc_home != nullptr,
                    "BUILDCC_HOME environment variable not defined");

  // NOTE, Verify BUILDCC_HOME
  // auto &buildcc_path = storage.Add<fs::path>("buildcc_path", buildcc_home);
  fs::path buildcc_home_path{buildcc_home};
  env::assert_fatal(fs::exists(buildcc_home_path),
                    "{BUILDCC_HOME} path not found");
  env::assert_fatal(fs::exists(buildcc_home_path / "buildcc"),
                    "{BUILDCC_HOME}/buildcc path not found");
  env::assert_fatal(fs::exists(buildcc_home_path / "libs"),
                    "{BUILDCC_HOME}/libs path not found");
  env::assert_fatal(fs::exists(buildcc_home_path / "extensions"),
                    "{BUILDCC_HOME}/extensions path not found");

  return buildcc_home_path;
}

} // namespace buildcc
