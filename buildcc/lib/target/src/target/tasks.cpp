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

constexpr const char *const kPchTaskName = "Pch";
constexpr const char *const kCompileTaskName = "Objects";
constexpr const char *const kLinkTaskName = "Target";

} // namespace

namespace buildcc::base {

void Pch::Task() {
  task_ = target_.tf_.emplace([&](tf::Subflow &subflow) {
    BuildCompile();

    // For Graph generation
    for (const auto &p : target_.GetCurrentPchFiles()) {
      std::string name =
          p.lexically_relative(env::get_project_root_dir()).string();
      std::replace(name.begin(), name.end(), '\\', '/');
      subflow.placeholder().name(name);
    }
  });
  task_.name(kPchTaskName);
}

void Target::ObjectTask() {
  env::log_trace(name_, __FUNCTION__);

  compile_task_ = tf_.emplace([&](tf::Subflow &subflow) {
    std::vector<fs::path> source_files;
    std::vector<fs::path> dummy_source_files;

    BuildObjectCompile(source_files, dummy_source_files);

    for (const auto &s : source_files) {
      std::string name =
          s.lexically_relative(env::get_project_root_dir()).string();
      std::replace(name.begin(), name.end(), '\\', '/');
      (void)subflow
          .emplace([this, s]() {
            bool success = Command::Execute(GetCompileCommand(s));
            env::assert_fatal(success, "Could not compile source");
          })
          .name(name);
    }

    // For graph generation
    for (const auto &ds : dummy_source_files) {
      std::string name =
          ds.lexically_relative(env::get_project_root_dir()).string();
      std::replace(name.begin(), name.end(), '\\', '/');
      (void)subflow.placeholder().name(name);
    }
  });
  compile_task_.name(kCompileTaskName);
}

void Target::TargetTask() {
  env::log_trace(name_, __FUNCTION__);

  link_task_ = tf_.emplace([&]() { BuildTargetLink(); });
  link_task_.name(kLinkTaskName);

  // Only add if not empty
  // PCH may not be used
  if (!pch_.GetTask().empty()) {
    compile_task_.succeed(pch_.GetTask());
  }
  link_task_.succeed(compile_task_);
}

} // namespace buildcc::base
