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

#include "env/assert_fatal.h"

#include <exception>
#include <thread>

namespace {

std::thread::id main_id = std::this_thread::get_id();

bool IsRunningOnThread() {
  bool threaded = true;
  if (std::this_thread::get_id() == main_id) {
    threaded = false;
  }
  return threaded;
}

} // namespace

namespace buildcc::env {

[[noreturn]] void assert_handle_fatal() {
  if (!IsRunningOnThread()) {
    std::exit(1);
  } else {
    throw std::exception();
  }
}

} // namespace buildcc::env
