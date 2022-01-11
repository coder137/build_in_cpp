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

#ifndef TARGET_API_SOURCE_API_H_
#define TARGET_API_SOURCE_API_H_

#include <filesystem>

namespace fs = std::filesystem;

namespace buildcc::base {

// Requires
// - TargetStorer
// - TargetState
// - TargetConfig
// - TargetEnv
template <typename T> class SourceApi {
public:
  void AddSourceAbsolute(const fs::path &absolute_source);
  void GlobSourcesAbsolute(const fs::path &absolute_source_dir);

  void AddSource(const fs::path &relative_source,
                 const fs::path &relative_to_target_path = "");
  void GlobSources(const fs::path &relative_to_target_path = "");
};

} // namespace buildcc::base

#endif
