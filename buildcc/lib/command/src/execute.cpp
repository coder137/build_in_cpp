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

bool Command::Execute(const std::string &command) {
  env::assert_fatal(!command.empty(),
                    fmt::format("Invalid command -> \"{}\"", command));
  buildcc::env::log_debug("system", command);

  tpl::Process process(command);
  return process.get_exit_status() == 0;
}

} // namespace buildcc
