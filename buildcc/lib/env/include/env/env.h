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

#ifndef ENV_ENV_H_
#define ENV_ENV_H_

#include <string>

#include <filesystem>

namespace fs = std::filesystem;

namespace buildcc {

class Project {
public:
  Project() = delete;
  Project(const Project &) = delete;
  Project(Project &&) = delete;
  static void Init(const fs::path &project_root_dir,
                   const fs::path &project_build_dir);
  static void Deinit();

  static bool IsInit();
  static const fs::path &GetRootDir();
  static const fs::path &GetBuildDir();

private:
  static bool &GetStaticInit();
  static fs::path &GetStaticRootDir();
  static fs::path &GetStaticBuildDir();
};

} // namespace buildcc

#endif
