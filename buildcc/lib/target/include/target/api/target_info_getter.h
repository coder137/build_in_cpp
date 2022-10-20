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

#ifndef TARGET_API_TARGET_INFO_GETTER_H_
#define TARGET_API_TARGET_INFO_GETTER_H_

#include "schema/path.h"

#include "target/common/target_state.h"

namespace buildcc::internal {

// Requires
// - TargetStorer
// - TargetEnv
// - TargetConfig
template <typename T> class TargetInfoGetter {
public:
  // Target Env
  const fs::path &GetTargetRootDir() const;
  const fs::path &GetTargetBuildDir() const;
};

} // namespace buildcc::internal

#endif
