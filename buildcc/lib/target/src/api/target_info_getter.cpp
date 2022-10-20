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

#include "target/api/target_info_getter.h"

#include "target/target_info.h"

namespace buildcc::internal {

// Target Env
template <typename T>
const fs::path &TargetInfoGetter<T>::GetTargetRootDir() const {
  const T &t = static_cast<const T &>(*this);

  return t.env_.GetTargetRootDir();
}

template <typename T>
const fs::path &TargetInfoGetter<T>::GetTargetBuildDir() const {
  const T &t = static_cast<const T &>(*this);

  return t.env_.GetTargetBuildDir();
}

template class TargetInfoGetter<TargetInfo>;

} // namespace buildcc::internal
