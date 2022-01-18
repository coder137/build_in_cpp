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

namespace buildcc {

void Target::SetTaskStateFailure() {
  std::lock_guard<std::mutex> guard(task_state_mutex_);
  task_state_ = env::TaskState::FAILURE;
}

tf::Task Target::CheckStateTask() {
  // NOTE, For now we only have 2 states
  // 0 -> SUCCESS
  // 1 -> FAILURE
  // * When more states are added make sure to handle them explicitly
  return tf_.emplace([&]() { return GetTaskStateAsInt(); })
      .name(kCheckTaskName);
}

void Target::StartTask() {
  // Return 0 for success
  // Return 1 for failure
  target_start_task_ = tf_.emplace([&]() {
    switch (env::get_task_state()) {
    case env::TaskState::SUCCESS:
      break;
    default:
      SetTaskStateFailure();
      break;
    };
    return GetTaskStateAsInt();
  });
  target_start_task_.name(kStartTaskName);
}

} // namespace buildcc

namespace buildcc::base {

// 1. User adds/removes/updates pch_headers
// 2. `BuildCompile` aggregates pch_headers to a single `buildcc_header` and
// compiles
// 3. Successfully compiled sources are added to `compiled_pch_files_`
void CompilePch::Task() {
  task_ = target_.tf_.emplace([&](tf::Subflow &subflow) {
    try {
      BuildCompile();
    } catch (...) {
      target_.SetTaskStateFailure();
    }

    // For Graph generation
    for (const auto &p : target_.GetPchFiles()) {
      std::string name =
          fmt::format("{}", p.lexically_relative(env::get_project_root_dir()));
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
    std::vector<internal::Path> selected_source_files;
    std::vector<internal::Path> selected_dummy_source_files;

    try {
      BuildObjectCompile(selected_source_files, selected_dummy_source_files);
      target_.compiled_source_files_.clear();
      target_.compiled_source_files_.insert(selected_dummy_source_files.begin(),
                                            selected_dummy_source_files.end());

      for (const auto &s : selected_source_files) {
        std::string name = fmt::format("{}", s.GetPathname().lexically_relative(
                                                 env::get_project_root_dir()));
        (void)subflow
            .emplace([this, s]() {
              try {
                bool success = env::Command::Execute(
                    GetObjectData(s.GetPathname()).command);
                env::assert_throw(success, "Could not compile source");

                // NOTE, If conmpilation is successful we update the source
                // files
                std::lock_guard<std::mutex> guard(
                    target_.compiled_source_files_mutex_);
                target_.compiled_source_files_.insert(s);
              } catch (...) {
                target_.SetTaskStateFailure();

                // NOTE, If compilation fails, we do not need to update the
                // source files
              }
            })
            .name(name);
      }

      // For graph generation
      for (const auto &ds : selected_dummy_source_files) {
        std::string name = fmt::format(
            "{}",
            ds.GetPathname().lexically_relative(env::get_project_root_dir()));
        (void)subflow.placeholder().name(name);
      }
    } catch (...) {
      target_.SetTaskStateFailure();
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
    try {
      BuildLink();
    } catch (...) {
      target_.SetTaskStateFailure();
    }
  });
  task_.name(kLinkTaskName);
}

} // namespace buildcc::base

namespace buildcc {

void Target::EndTask() {
  target_end_task_ = tf_.emplace([&]() {
    if (dirty_) {
      try {
        storer_.current_source_files.internal = compiled_source_files_;
        env::assert_throw(Store(),
                          fmt::format("Store failed for {}", GetName()));
        state_.build = true;
      } catch (...) {
        SetTaskStateFailure();
      }
    }

    // Update env task state
    if (task_state_ != env::TaskState::SUCCESS) {
      env::set_task_state(GetTaskState());
    }
  });
  target_end_task_.name(kEndTaskName);
}

void Target::TaskDeps() {
  if (state_.ContainsPch()) {
    target_start_task_.precede(compile_pch_.GetTask(), target_end_task_);
    tf::Task pch_check_state_task = CheckStateTask();
    compile_pch_.GetTask().precede(pch_check_state_task);
    pch_check_state_task.precede(compile_object_.GetTask(), target_end_task_);
  } else {
    target_start_task_.precede(compile_object_.GetTask(), target_end_task_);
  }

  tf::Task object_check_state_task = CheckStateTask();
  compile_object_.GetTask().precede(object_check_state_task);
  object_check_state_task.precede(link_target_.GetTask(), target_end_task_);
  link_target_.GetTask().precede(target_end_task_);
}

} // namespace buildcc
