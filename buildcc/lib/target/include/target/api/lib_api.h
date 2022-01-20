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

#ifndef TARGET_API_LIB_API_H_
#define TARGET_API_LIB_API_H_

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

namespace buildcc {

class Target;

}

namespace buildcc::internal {

// Requires
// - TargetStorer
// - TargetState
// - TargetEnv
// T::GetTargetPath
template <typename T> class LibApi {
public:
  void AddLibDep(const Target &lib_dep);
  void AddLibDep(const std::string &lib_dep);

  void AddLibDir(const fs::path &relative_lib_dir);
  void AddLibDirAbsolute(const fs::path &absolute_lib_dir);
};

} // namespace buildcc::internal

#endif
