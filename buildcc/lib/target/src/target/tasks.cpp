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

#include "target/target.h"

#include <algorithm>

#include "env/assert_fatal.h"
#include "env/logging.h"

#include "target/util.h"

#include "fmt/format.h"

namespace {

constexpr const char *const kCompileTaskName = "Compile";
constexpr const char *const kLinkTaskName = "Link";

} // namespace

namespace buildcc::base {

void Target::CompileTask() {
  env::log_trace(name_, __FUNCTION__);

  BuildCompileGenerator();
  compile_task_ = tf_.emplace(
      [&](tf::Subflow &subflow) { compile_generator_.Build(subflow); });
  compile_task_.name(kCompileTaskName);
}

void Target::LinkTask() {
  env::log_trace(name_, __FUNCTION__);

  BuildLinkGenerator();
  link_task_ = tf_.emplace(
      [&](tf::Subflow &subflow) { link_generator_.Build(subflow); });

  link_task_.name(kLinkTaskName);
  link_task_.succeed(compile_task_);
}

} // namespace buildcc::base
