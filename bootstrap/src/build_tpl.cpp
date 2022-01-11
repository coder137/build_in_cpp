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

#include "bootstrap/build_tpl.h"

namespace buildcc {

void tpl_cb(BaseTarget &target, const TplConfig &config) {
  target.AddSource("process.cpp");
  target.AddIncludeDir("");
  target.AddHeader("process.hpp");

  switch (config.os_id) {
  case OsId::Win:
    target.AddSource("process_win.cpp");
    break;
  case OsId::Linux:
  case OsId::Unix:
  case OsId::Mac:
    target.AddSource("process_unix.cpp");
    break;
  default:
    break;
  }

  target.Build();
}

} // namespace buildcc
