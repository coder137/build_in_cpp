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

#ifndef BUILDEXE_BUILD_ENV_HOME_H_
#define BUILDEXE_BUILD_ENV_HOME_H_

#include "buildcc.h"

namespace buildcc {

class BuildccHome {
public:
  static void Init();

  static const fs::path &GetBuildccHomeDir() {
    ExpectInitialized();
    return buildcc_home_;
  }
  static const fs::path &GetBuildccBaseDir() {
    ExpectInitialized();
    return buildcc_base_;
  }
  static const fs::path &GetBuildccLibsDir() {
    ExpectInitialized();
    return buildcc_libs_;
  }
  static const fs::path &GetBuildccExtensionsDir() {
    ExpectInitialized();
    return buildcc_extensions_;
  }

  static bool IsInitialized() { return initialized_; }
  static void ExpectInitialized() {
    env::assert_fatal(IsInitialized(), "BuildccHome is not initialized");
  }

private:
  static fs::path buildcc_home_;
  static fs::path buildcc_base_;
  static fs::path buildcc_libs_;
  static fs::path buildcc_extensions_;
  static bool initialized_;
};

} // namespace buildcc

#endif
