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

#include "command/command.h"

#include "fmt/format.h"

#include "env/assert_fatal.h"
#include "env/logging.h"

#include "process.hpp"

namespace tpl = TinyProcessLib;

namespace buildcc {

bool Command::Execute(const std::string &command,
                      std::vector<std::string> *stdout_data,
                      std::vector<std::string> *stderr_data) {
  env::assert_fatal(!command.empty(), "Empty command");
  buildcc::env::log_debug("system", command);

  std::function<void(const char *bytes, size_t n)> stdout_func =
      [&](const char *bytes, size_t n) {
        stdout_data->emplace_back(std::string(bytes, n));
      };

  std::function<void(const char *bytes, size_t n)> stderr_func =
      [&](const char *bytes, size_t n) {
        stderr_data->emplace_back(std::string(bytes, n));
      };

  tpl::Process process(command, tpl::Process::string_type(),
                       stdout_data == nullptr ? nullptr : stdout_func,
                       stderr_data == nullptr ? nullptr : stderr_func);
  return process.get_exit_status() == 0;
}

} // namespace buildcc
