/*
 * Copyright 2021 Niket Naidu All rights reserved.
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

#include "internal/util.h"

#include "logging.h"

namespace buildcc::internal {

// command
bool command(const std::vector<std::string> &command_tokens) {
  std::string command = aggregate(command_tokens);
  buildcc::env::log_debug("system", command);
  return system(command.c_str()) == 0;
}

} // namespace buildcc::internal
