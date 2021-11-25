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

#include "env/private/task_state.h"

namespace {

buildcc::env::TaskState current_state{buildcc::env::TaskState::SUCCESS};

}

namespace buildcc::env {

void set_task_state(TaskState state) { current_state = state; }
bool is_task_state_success() {
  if (current_state == TaskState::SUCCESS) {
    return true;
  }
  return false;
}

} // namespace buildcc::env
