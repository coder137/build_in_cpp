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

#include "internal/util.h"

#include <iostream>

#include "logging.h"

#include "process.hpp"

namespace tpl = TinyProcessLib;

namespace buildcc::internal {

// command
bool command(const std::vector<std::string> &command_tokens) {
  std::string command = aggregate(command_tokens);
  buildcc::env::log_debug("system", command);
  // return system(command.c_str()) == 0;

  tpl::Process process(
      command_tokens, "",
      [](const char *bytes, size_t n) {
        // env::log_info("STDOUT", std::string(bytes, n));
        std::cout << "STDOUT " << std::string(bytes, n) << std::endl;
      },
      [](const char *bytes, size_t n) {
        // env::log_warning("STDERR", std::string(bytes, n));
        std::cerr << "STDERR " << std::string(bytes, n) << std::endl;
      });

  return process.get_exit_status() == 0;
}

} // namespace buildcc::internal
