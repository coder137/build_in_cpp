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

#include "bootstrap/buildcc_tpl.h"

namespace buildcc {

void tpl_cb(BaseTarget &target) {
  target.AddSource("process.cpp");
  target.AddIncludeDir("");
  target.AddHeader("process.hpp");

  // MinGW (GCC), MSVC, Clang
  if constexpr (env::is_win()) {
    target.AddSource("process_win.cpp");
    // TODO, MSVC
    // TODO, Clang
    switch (target.GetToolchain().GetId()) {
    case ToolchainId::Gcc:
    case ToolchainId::MinGW:
      target.AddCppCompileFlag("-std=c++17");
      target.AddCppCompileFlag("-Wall");
      target.AddCppCompileFlag("-Wextra");
      break;
    default:
      break;
    }
  }

  if constexpr (env::is_linux()) {
    // TODO, GCC
    // TODO, Clang
  }

  // LOG DUMP
  for (const auto &d : target.GetCurrentSourceFiles()) {
    env::log_info(__FUNCTION__, d.string());
  }

  target.Build();
}

} // namespace buildcc
