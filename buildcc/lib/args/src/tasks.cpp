/*
 * Copyright 2021 Niket Naidu. All rights reserved.
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

#include "args/register.h"
#include "env/logging.h"

namespace buildcc {

tf::Task Register::BuildTask(base::Target &target) {
  return tf_.composed_of(target.GetTaskflow()).name(target.GetUniqueId());
}

void Register::RunBuild() {
  env::log_info(__FUNCTION__, fmt::format("Running with {} workers",
                                          executor_.num_workers()));
  executor_.run(tf_);
  executor_.wait_for_all();
}

} // namespace buildcc
