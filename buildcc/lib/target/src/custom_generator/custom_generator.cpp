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

#include "target/custom_generator.h"

namespace {

constexpr const char *const kGenerateTaskName = "Generate";
constexpr const char *const kProjectRootDirName = "project_root_dir";
constexpr const char *const kProjectBuildDirName = "project_build_dir";
constexpr const char *const kCurrentRootDirName = "current_root_dir";
constexpr const char *const kCurrentBuildDirName = "current_build_dir";

} // namespace

namespace buildcc {

struct Comparator {
  Comparator(const buildcc::internal::CustomGeneratorSchema &loaded,
             const buildcc::UserCustomGeneratorSchema &current)
      : loaded_schema_(loaded), current_schema_(current) {}

  enum class State {
    kRemoved,
    kAdded,
    kCheckLater,
  };

  void AddAllIds() {
    const auto &curr_ids = current_schema_.ids;
    for (const auto &[id, _] : curr_ids) {
      id_state_info_.at(State::kAdded).insert(id);
    }
  }

  void CompareAndAddIds() {
    const auto &prev_ids = loaded_schema_.internal_ids;
    const auto &curr_ids = current_schema_.ids;

    for (const auto &[prev_id, _] : prev_ids) {
      if (curr_ids.find(prev_id) == curr_ids.end()) {
        // Id Removed condition, previous id is not present in the current run
        id_state_info_.at(State::kRemoved).insert(prev_id);
      }
    }

    for (const auto &[curr_id, _] : curr_ids) {
      if (prev_ids.find(curr_id) == prev_ids.end()) {
        // Id Added condition
        id_state_info_.at(State::kAdded).insert(curr_id);
      } else {
        // Id Check Later condition
        id_state_info_.at(State::kCheckLater).insert(curr_id);
      }
    }
  }

  bool IsChanged(const std::string &id) const {
    const auto &previous_id_info = loaded_schema_.internal_ids.at(id);
    const auto &current_id_info = current_schema_.ids.at(id);

    bool changed = !previous_id_info.inputs.IsEqual(current_id_info.inputs) ||
                   !previous_id_info.outputs.IsEqual(current_id_info.outputs);
    if (!changed && current_id_info.blob_handler != nullptr) {
      // We only check blob handler if not changed by inputs/outputs
      // Checking blob_handler could be expensive so this optimization is made
      // to run only when changed == false
      changed = current_id_info.blob_handler->CheckChanged(
          previous_id_info.userblob, current_id_info.userblob);
    }
    return changed;
  }

  const std::unordered_set<std::string> &GetRemovedIds() const {
    return id_state_info_.at(State::kRemoved);
  }

  const std::unordered_set<std::string> &GetAddedIds() const {
    return id_state_info_.at(State::kAdded);
  }

  const std::unordered_set<std::string> &GetCheckLaterIds() const {
    return id_state_info_.at(State::kCheckLater);
  }

  bool IsIdAdded(const std::string &id) const {
    return id_state_info_.at(State::kAdded).count(id) == 1;
  }

private:
  const buildcc::internal::CustomGeneratorSchema &loaded_schema_;
  const buildcc::UserCustomGeneratorSchema &current_schema_;
  std::unordered_map<State, std::unordered_set<std::string>> id_state_info_{
      {State::kRemoved, std::unordered_set<std::string>()},
      {State::kAdded, std::unordered_set<std::string>()},
      {State::kCheckLater, std::unordered_set<std::string>()},
  };
};

struct TaskState {
  bool should_run{false};
  bool run_success{false};
};

struct TaskFunctor {
  TaskFunctor(const std::string &id,
              UserCustomGeneratorSchema::UserIdInfo &id_info,
              const Comparator &comparator, const env::Command &command,
              TaskState &state)
      : id_(id), id_info_(id_info), comparator(comparator), command_(command),
        state_(state) {}

  void operator()() {
    if (env::get_task_state() != env::TaskState::SUCCESS) {
      return;
    }
    try {
      id_info_.ConvertToInternal();
      // Compute runnable
      state_.should_run =
          comparator.IsIdAdded(id_) ? true : comparator.IsChanged(id_);

      // Invoke generator callback
      if (state_.should_run) {
        const auto input_paths = id_info_.inputs.GetPaths();
        CustomGeneratorContext ctx(command_, input_paths,
                                   id_info_.outputs.GetPaths(),
                                   id_info_.userblob);

        bool success = id_info_.generate_cb(ctx);
        env::assert_fatal(success,
                          fmt::format("Generate Cb failed for id {}", id_));
      }
      state_.run_success = true;
    } catch (...) {
      env::set_task_state(env::TaskState::FAILURE);
    }
  }

private:
  const std::string &id_;
  UserCustomGeneratorSchema::UserIdInfo &id_info_;

  const Comparator &comparator;
  const env::Command &command_;

  TaskState &state_;
};

bool ComputeBuild(const internal::CustomGeneratorSerialization &serialization,
                  Comparator &comparator,
                  std::function<void(void)> &&id_removed_cb,
                  std::function<void(void)> &&id_added_cb) {
  bool build = false;
  if (!serialization.IsLoaded()) {
    comparator.AddAllIds();
    build = true;
  } else {
    comparator.CompareAndAddIds();
    const bool is_removed = !comparator.GetRemovedIds().empty();
    const bool is_added = !comparator.GetAddedIds().empty();
    build = is_removed || is_added;

    if (is_removed) {
      id_removed_cb();
    }

    for (const auto &id : comparator.GetAddedIds()) {
      (void)id;
      id_added_cb();
    }
  }
  return build;
}

void CustomGenerator::AddPattern(const std::string &identifier,
                                 const std::string &pattern) {
  command_.AddDefaultArgument(identifier, command_.Construct(pattern));
}

void CustomGenerator::AddPatterns(
    const std::unordered_map<std::string, std::string> &pattern_map) {
  for (const auto &[identifier, pattern] : pattern_map) {
    AddPattern(identifier, pattern);
  }
}

std::string CustomGenerator::ParsePattern(
    const std::string &pattern,
    const std::unordered_map<const char *, std::string> &arguments) const {
  return command_.Construct(pattern, arguments);
}

const std::string &
CustomGenerator::Get(const std::string &file_identifier) const {
  return command_.GetDefaultValueByKey(file_identifier);
}

void CustomGenerator::AddIdInfo(
    const std::string &id, const std::unordered_set<std::string> &inputs,
    const std::unordered_set<std::string> &outputs,
    const GenerateCb &generate_cb,
    const std::shared_ptr<CustomBlobHandler> &blob_handler) {
  env::assert_fatal(user_.ids.find(id) == user_.ids.end(),
                    fmt::format("Duplicate id {} detected", id));
  ASSERT_FATAL(generate_cb, "Invalid callback provided");

  UserCustomGeneratorSchema::UserIdInfo schema;
  for (const auto &i : inputs) {
    auto input = command_.Construct(i);
    schema.inputs.Emplace(input, "");
  }
  for (const auto &o : outputs) {
    auto output = command_.Construct(o);
    schema.outputs.Emplace(output);
  }
  schema.generate_cb = generate_cb;
  schema.blob_handler = blob_handler;
  user_.ids.try_emplace(id, std::move(schema));
}

void CustomGenerator::Build() {
  (void)serialization_.LoadFromFile();
  GenerateTask();
}

// PRIVATE
void CustomGenerator::Initialize() {
  // Checks
  env::assert_fatal(
      Project::IsInit(),
      "Environment is not initialized. Use the buildcc::Project::Init API");

  //
  fs::create_directories(env_.GetTargetBuildDir());
  command_.AddDefaultArguments({
      {kProjectRootDirName, path_as_string(Project::GetRootDir())},
      {kProjectBuildDirName, path_as_string(Project::GetBuildDir())},
      {kCurrentRootDirName, path_as_string(env_.GetTargetRootDir())},
      {kCurrentBuildDirName, path_as_string(env_.GetTargetBuildDir())},
  });

  //
  unique_id_ = name_;
  tf_.name(name_);
}

void CustomGenerator::GenerateTask() {
  tf::Task generate_task = tf_.emplace([&](tf::Subflow &subflow) {
    if (env::get_task_state() != env::TaskState::SUCCESS) {
      return;
    }

    try {
      // Selected ids for build
      Comparator comparator(serialization_.GetLoad(), user_);
      dirty_ = ComputeBuild(
          serialization_, comparator, [this]() { IdRemoved(); },
          [this]() { IdAdded(); });

      std::unordered_map<std::string, TaskState> states;

      // Create runner for each added/updated id
      for (const auto &id : comparator.GetAddedIds()) {
        states.try_emplace(id, TaskState());
        auto &id_info = user_.ids.at(id);
        TaskFunctor functor(id, id_info, comparator, command_, states.at(id));
        subflow.emplace(functor).name(id);
      }

      for (const auto &id : comparator.GetCheckLaterIds()) {
        states.try_emplace(id, TaskState());
        auto &id_info = user_.ids.at(id);
        TaskFunctor functor(id, id_info, comparator, command_, states.at(id));
        subflow.emplace(functor).name(id);
      }

      // NOTE, Do not call detach otherwise this will fail
      subflow.join();

      UserCustomGeneratorSchema user_final_schema;
      for (const auto &[id, state] : states) {
        dirty_ = dirty_ || state.should_run;
        if (state.run_success) {
          user_final_schema.ids.try_emplace(id, user_.ids.at(id));
        }
      }

      // Store
      if (dirty_) {
        user_final_schema.ConvertToInternal();

        serialization_.UpdateStore(user_final_schema);
        env::assert_fatal(serialization_.StoreToFile(),
                          fmt::format("Store failed for {}", name_));
      }
    } catch (...) {
      env::set_task_state(env::TaskState::FAILURE);
    }
  });
  generate_task.name(kGenerateTaskName);
}

} // namespace buildcc
