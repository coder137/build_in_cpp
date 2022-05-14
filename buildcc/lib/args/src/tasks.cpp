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

#include "args/register.h"
#include "env/logging.h"
#include "env/util.h"

namespace buildcc {

tf::Task Reg::Instance::BuildTask(BaseTarget &target) {
  return build_tf_.composed_of(target.GetTaskflow()).name(target.GetUniqueId());
}

tf::Task Reg::Instance::BuildTask(BaseGenerator &generator) {
  return build_tf_.composed_of(generator.GetTaskflow())
      .name(generator.GetUniqueId());
}

void Reg::Instance::RunBuild() {
  tf::Executor executor;
  env::log_info(__FUNCTION__,
                fmt::format("Running with {} workers", executor.num_workers()));
  executor.run(build_tf_);
  executor.wait_for_all();
  env::assert_fatal(env::get_task_state() == env::TaskState::SUCCESS,
                    "Task state is not successful!");
}

void Reg::Instance::RunTest() {
  tf::Taskflow test_tf{"Tests"};
  test_tf.for_each(
      tests_.begin(), tests_.end(),
      [](const std::pair<std::string, TestInfo> &p) { p.second.TestRunner(); });

  tf::Executor executor;
  executor.run(test_tf);
  executor.wait_for_all();
}

} // namespace buildcc
