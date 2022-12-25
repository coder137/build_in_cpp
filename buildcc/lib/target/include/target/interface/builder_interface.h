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

#ifndef TARGET_INTERFACE_BUILDER_INTERFACE_H_
#define TARGET_INTERFACE_BUILDER_INTERFACE_H_

#include <algorithm>
#include <functional>
#include <unordered_set>

#include "taskflow/taskflow.hpp"

#include "env/assert_fatal.h"

#include "target/common/util.h"

namespace buildcc::internal {

class BuilderInterface {

public:
  virtual void Build() = 0;

  const std::string &GetUniqueId() const { return unique_id_; }
  tf::Taskflow &GetTaskflow() { return tf_; }

protected:
  bool dirty_{false};
  std::string unique_id_;
  tf::Taskflow tf_;
};

} // namespace buildcc::internal

#endif
