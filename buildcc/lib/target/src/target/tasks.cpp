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

#include "env/logging.h"

#include "target/common/util.h"

#include "fmt/format.h"

namespace {

constexpr const char *const kPchTaskName = "Pch";
constexpr const char *const kCompileTaskName = "Objects";
constexpr const char *const kLinkTaskName = "Target";

} // namespace

namespace buildcc::base {

void Target::SetTaskStateFailure() {
  if (task_state_ != env::TaskState::FAILURE) {
    std::lock_guard<std::mutex> guard(task_state_mutex_);
    task_state_ = env::TaskState::FAILURE;
  }
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
  target_start_task_.name("Start Target");
}

tf::Task Target::CheckStateTask() {
  // NOTE, For now we only have 2 states
  // 0 -> SUCCESS
  // 1 -> FAILURE
  // * When more states are added make sure to handle them explicitly
  return tf_.emplace([&]() { return GetTaskStateAsInt(); })
      .name("Check Target");
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

// TODO, Shift this to its own source file
void CompilePch::Task() {
  task_ = target_.tf_.emplace([&](tf::Subflow &subflow) {
    try {
      BuildCompile();
      target_.pch_files_ = target_.storer_.current_pch_files.internal;
    } catch (...) {
      target_.SetTaskStateFailure();
    }

    // For Graph generation
    for (const auto &p : target_.GetCurrentPchFiles()) {
      std::string name =
          fmt::format("{}", p.lexically_relative(env::get_project_root_dir()));
      subflow.placeholder().name(name);
    }
  });
  task_.name(kPchTaskName);
}

// TODO, Shift this to its own source file
void CompileObject::Task() {
  compile_task_ = target_.tf_.emplace([&](tf::Subflow &subflow) {
    std::vector<internal::Path> source_files;
    std::vector<internal::Path> dummy_source_files;

    try {
      BuildObjectCompile(source_files, dummy_source_files);
      target_.source_files_.clear();
      target_.source_files_.insert(dummy_source_files.begin(),
                                   dummy_source_files.end());

      for (const auto &s : source_files) {
        std::string name = fmt::format("{}", s.GetPathname().lexically_relative(
                                                 env::get_project_root_dir()));
        (void)subflow
            .emplace([this, s]() {
              try {
                bool success =
                    Command::Execute(GetObjectData(s.GetPathname()).command);
                env::assert_throw(success, "Could not compile source");

                // NOTE, If conmpilation is successful we update the source
                // files
                std::lock_guard<std::mutex> guard(
                    target_.update_path_file_mutex_);
                target_.source_files_.insert(s);
              } catch (...) {
                target_.SetTaskStateFailure();

                // NOTE, If compilation fails, we do not need to update the
                // source files
              }
            })
            .name(name);

        // For graph generation
        for (const auto &ds : dummy_source_files) {
          std::string name = fmt::format(
              "{}",
              ds.GetPathname().lexically_relative(env::get_project_root_dir()));
          (void)subflow.placeholder().name(name);
        }
      }
    } catch (...) {
      target_.SetTaskStateFailure();
    }
  });
  compile_task_.name(kCompileTaskName);
}

// TODO, Shift this to its own source file
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

void Target::EndTask() {
  target_end_task_ = tf_.emplace([&]() {
    if (dirty_) {
      try {
        storer_.current_pch_files.internal = pch_files_;
        storer_.current_source_files.internal = source_files_;
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
  target_end_task_.name("End Target");
}

} // namespace buildcc::base
