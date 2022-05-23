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

#include "target/target.h"

#include <algorithm>

#include "env/logging.h"

#include "target/common/util.h"

#include "fmt/format.h"

namespace {

constexpr const char *const kStartTaskName = "Start Target";
constexpr const char *const kEndTaskName = "End Target";
constexpr const char *const kCheckTaskName = "Check Target";

constexpr const char *const kPchTaskName = "Pch";
constexpr const char *const kCompileTaskName = "Objects";
constexpr const char *const kLinkTaskName = "Target";

} // namespace

namespace buildcc {} // namespace buildcc

namespace buildcc::internal {

// 1. User adds/removes/updates pch_headers
// 2. `BuildCompile` aggregates pch_headers to a single `buildcc_header` and
// compiles
// 3. Successfully compiled sources are added to `compiled_pch_files_`
void CompilePch::Task() {
  task_ = target_.tf_.emplace([&](tf::Subflow &subflow) {
    if (env::get_task_state() != env::TaskState::SUCCESS) {
      return;
    }

    try {
      BuildCompile();
      target_.serialization_.UpdatePchCompiled(target_.user_);
    } catch (...) {
      env::set_task_state(env::TaskState::FAILURE);
    }

    // For Graph generation
    for (const auto &p : target_.GetPchFiles()) {
      std::string name =
          fmt::format("{}", p.lexically_relative(Project::GetRootDir()));
      subflow.placeholder().name(name);
    }
  });
  task_.name(kPchTaskName);
}

// 1. User adds/removes/updates sources (user_source_files)
// 2. `BuildObjectCompile` populates `selected_source_files` that need to be
// compiled
// 3. Successfully compiled sources are added to `compiled_source_files_`
// * `selected_source_files` can be a subset of `user_source_files`
// * `compiled_source_files` can be a subset of `selected_source_files`
// NOTE, We do not use state here since we are compiling every source file
// individually
// We would need to store `source_file : object_file : state` in our
// serialization schema
void CompileObject::Task() {
  compile_task_ = target_.tf_.emplace([&](tf::Subflow &subflow) {
    if (env::get_task_state() != env::TaskState::SUCCESS) {
      return;
    }

    std::vector<internal::Path> selected_source_files;
    std::vector<internal::Path> selected_dummy_source_files;

    try {
      BuildObjectCompile(selected_source_files, selected_dummy_source_files);
      for (const auto &piter : selected_dummy_source_files) {
        target_.serialization_.AddSource(piter);
      }

      for (const auto &s : selected_source_files) {
        std::string name = fmt::format(
            "{}", s.GetPathname().lexically_relative(Project::GetRootDir()));
        (void)subflow
            .emplace([this, s]() {
              try {
                bool success = env::Command::Execute(
                    GetObjectData(s.GetPathname()).command);
                env::assert_fatal(success, "Could not compile source");
                target_.serialization_.AddSource(s);
              } catch (...) {
                env::set_task_state(env::TaskState::FAILURE);
              }
            })
            .name(name);
      }

      // For graph generation
      for (const auto &ds : selected_dummy_source_files) {
        std::string name = fmt::format(
            "{}", ds.GetPathname().lexically_relative(Project::GetRootDir()));
        (void)subflow.placeholder().name(name);
      }
    } catch (...) {
      env::set_task_state(env::TaskState::FAILURE);
    }
  });
  compile_task_.name(kCompileTaskName);
}

// 1. Receives object list from compile stage (not serialized)
// 2. `BuildLink` links compiled objects and other user supplied parameters to
// the required target
// 3. Successfully linking the target sets link state
void LinkTarget::Task() {
  task_ = target_.tf_.emplace([&]() {
    if (env::get_task_state() != env::TaskState::SUCCESS) {
      return;
    }
    try {
      BuildLink();
    } catch (...) {
      env::set_task_state(env::TaskState::FAILURE);
    }
  });
  task_.name(kLinkTaskName);
}

} // namespace buildcc::internal

namespace buildcc {

void Target::EndTask() {
  target_end_task_ = tf_.emplace([&]() {
    if (dirty_) {
      try {
        serialization_.UpdateStore(user_);
        env::assert_fatal(serialization_.StoreToFile(),
                          fmt::format("Store failed for {}", GetName()));
        state_.BuildCompleted();
      } catch (...) {
        env::set_task_state(env::TaskState::FAILURE);
      }
    }
  });
  target_end_task_.name(kEndTaskName);
}

void Target::TaskDeps() {
  if (state_.ContainsPch()) {
    compile_pch_.GetTask().precede(compile_object_.GetTask());
  }
  compile_object_.GetTask().precede(link_target_.GetTask());
  link_target_.GetTask().precede(target_end_task_);
}

} // namespace buildcc
