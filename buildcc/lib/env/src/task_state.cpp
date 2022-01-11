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

#include "env/task_state.h"

#include <mutex>

namespace {

std::mutex current_state_mutex;
buildcc::env::TaskState current_state{buildcc::env::TaskState::SUCCESS};

} // namespace

namespace buildcc::env {

void set_task_state(TaskState state) {
  // NOTE, `Avoid resetting` if same state is provided
  if (state == get_task_state()) {
    return;
  }
  std::lock_guard<std::mutex> guard(current_state_mutex);
  current_state = state;
}

TaskState get_task_state() { return current_state; }

} // namespace buildcc::env
