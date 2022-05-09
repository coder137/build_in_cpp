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
constexpr const char *const kStartGeneratorTaskName = "Start Generator";
constexpr const char *const kEndGeneratorTaskName = "End Generator";

constexpr const char *const kCommandTaskName = "Command";
constexpr const char *const kGenerateTaskName = "Generate";

} // namespace

namespace buildcc {

void CustomGenerator::AddDefaultArguments(
    const std::unordered_map<std::string, std::string> &arguments) {
  command_.AddDefaultArguments(arguments);
}

void CustomGenerator::AddGenInfo(const std::string &id,
                                 const fs_unordered_set &inputs,
                                 const fs_unordered_set &outputs,
                                 const GenerateCb &generate_cb) {
  env::assert_fatal(user_.rels_map.find(id) == user_.rels_map.end(),
                    fmt::format("Duplicate id {} detected", id));
  ASSERT_FATAL(generate_cb, "Invalid callback provided");

  UserRelInputOutputSchema schema;
  for (const auto &i : inputs) {
    schema.inputs.emplace(command_.Construct(path_as_string(i)));
  }
  for (const auto &o : outputs) {
    schema.outputs.emplace(command_.Construct(path_as_string(o)));
  }
  schema.generate_cb = generate_cb;
  user_.rels_map.emplace(id, std::move(schema));
}

void CustomGenerator::AddDependencyCb(const DependencyCb &dependency_cb) {
  dependency_cb_ = dependency_cb;
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
      {"gen_root_dir", path_as_string(env_.GetTargetRootDir())},
      {"gen_build_dir", path_as_string(env_.GetTargetBuildDir())},
  });

  //
  unique_id_ = name_;
  tf_.name(name_);
}

void CustomGenerator::BuildGenerate(
    std::unordered_map<std::string, UserRelInputOutputSchema> &gen_selected_map,
    std::unordered_map<std::string, UserRelInputOutputSchema>
        &dummy_gen_selected_map) {
  if (!serialization_.IsLoaded()) {
    gen_selected_map = user_.rels_map;
    dirty_ = true;
  } else {
    // DONE, Conditionally select internal_rels depending on what has changed
    const auto &prev_rels = serialization_.GetLoad().internal_rels_map;
    const auto &curr_rels = user_.rels_map;

    // DONE, MAP REMOVED condition Check if prev_rels exists in curr_rels
    // If prev_rels does not exist in curr_rels, has been removed from existing
    // build
    // We need this condition to only set the dirty_ flag
    for (const auto &prev_miter : prev_rels) {
      const auto &id = prev_miter.first;
      if (curr_rels.find(id) == curr_rels.end()) {
        // MAP REMOVED condition
        IdRemoved();
        dirty_ = true;
        break;
      }
    }

    // DONE, MAP ADDED condition Check if curr_rels exists in prev_rels
    // If curr_rels does not exist in prev_rels, has been added to existing
    // build
    for (const auto &curr_miter : curr_rels) {
      const auto &id = curr_miter.first;
      if (prev_rels.find(id) == prev_rels.end()) {
        // MAP ADDED condition
        IdAdded();
        gen_selected_map.emplace(curr_miter.first, curr_miter.second);
        dirty_ = true;
      } else {
        // MAP UPDATED condition (*checked later)
        // This is because tasks can have dependencies amongst each other we can
        // compute task level rebuilds later
        dummy_gen_selected_map.emplace(curr_miter.first, curr_miter.second);
      }
    }
  }
}

void CustomGenerator::GenerateTask() {
  tf::Task generate_task = tf_.emplace([&](tf::Subflow &subflow) {
    if (env::get_task_state() != env::TaskState::SUCCESS) {
      return;
    }

    try {
      std::unordered_map<std::string, UserRelInputOutputSchema>
          selected_user_schema;
      std::unordered_map<std::string, UserRelInputOutputSchema>
          dummy_selected_user_schema;
      BuildGenerate(selected_user_schema, dummy_selected_user_schema);

      std::unordered_map<std::string, tf::Task> task_map;
      // Create task for selected schema
      for (const auto &selected_miter : selected_user_schema) {
        const auto &id = selected_miter.first;
        tf::Task task = subflow
                            .emplace([&]() {
                              try {
                                TaskRunner<true>(id);
                              } catch (...) {
                                env::set_task_state(env::TaskState::FAILURE);
                              }
                            })
                            .name(id);
        task_map.emplace(id, task);
      }

      for (auto &dummy_selected_miter : dummy_selected_user_schema) {
        const auto &id = dummy_selected_miter.first;
        auto &current_info = dummy_selected_miter.second;
        tf::Task task = subflow
                            .emplace([&]() {
                              try {
                                user_.rels_map.at(id).internal_inputs =
                                    internal::path_schema_convert(
                                        current_info.inputs,
                                        internal::Path::CreateExistingPath);
                                TaskRunner<false>(id);
                              } catch (...) {
                                env::set_task_state(env::TaskState::FAILURE);
                              }
                            })
                            .name(id);
        task_map.emplace(id, task);
      }

      // Dependencies between ids
      if (dependency_cb_) {
        dependency_cb_(task_map);
      }

      // NOTE, Do not call detach otherwise this will fail
      subflow.join();

      // Store dummy_selected and successfully run schema
      if (dirty_) {
        UserCustomGeneratorSchema user_final_schema;
        user_final_schema.rels_map.insert(success_schema_.begin(),
                                          success_schema_.end());

        // TODO, Update this
        user_final_schema.ConvertToInternal();
        serialization_.UpdateStore(user_final_schema);
        env::assert_fatal(serialization_.StoreToFile(),
                          fmt::format("Store failed for {}", name_));
      }

    } catch (...) {
      env::set_task_state(env::TaskState::FAILURE);
    }
  });
  // TODO, Instead of "Generate" name the task of user's choice
  generate_task.name(kGenerateTaskName);
}

template <bool run> void CustomGenerator::TaskRunner(const std::string &id) {
  const auto &current_info = user_.rels_map.at(id);
  bool rerun = false;
  if constexpr (run) {
    rerun = true;
  } else {
    const auto &previous_info =
        serialization_.GetLoad().internal_rels_map.at(id);
    rerun = internal::CheckPaths(previous_info.internal_inputs,
                                 current_info.internal_inputs) !=
                internal::PathState::kNoChange ||
            internal::CheckChanged(previous_info.outputs, current_info.outputs);
  }

  if (rerun) {
    buildcc::CustomGeneratorContext ctx(command_, current_info.inputs,
                                        current_info.outputs);
    bool success = current_info.generate_cb(ctx);
    env::assert_fatal(success, fmt::format("Generate Cb failed for id {}", id));
  }

  std::lock_guard<std::mutex> guard(success_schema_mutex_);
  success_schema_.emplace(id, current_info);
}

} // namespace buildcc
