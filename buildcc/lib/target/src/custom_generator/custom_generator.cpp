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

#include "target/generator.h"

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

void CustomGenerator::Convert() { user_.ConvertToInternal(); }

void CustomGenerator::BuildGenerate(
    std::unordered_map<std::string, UserRelInputOutputSchema> &gen_selected_map,
    std::unordered_map<std::string, UserRelInputOutputSchema>
        &dummy_gen_selected_map) {
  if (!serialization_.IsLoaded()) {
    gen_selected_map = user_.rels_map;
    dirty_ = true;
  } else {
    // DONE, Conditionally select internal_rels depending on what has changed
    const auto &curr_rels = user_.internal_rels_map;
    const auto &prev_rels = serialization_.GetLoad().internal_rels_map;

    // TODO, MAP REMOVED condition Check if prev_rels exists in curr_rels
    // If prev_rels does not exist in curr_rels, has been removed from existing
    // build
    // We need this condition to only set the dirty_ flag

    // DONE, MAP ADDED condition Check if curr_rels exists in prev_rels
    // If curr_rels does not exist in prev_rels, has been added to existing
    // build

    // DONE, MAP UPDATED condition, Check if curr_rels exists in prev_rels
    // If curr_rels exists in prev_rels, but the
    // * INPUT has been removed, added or changed
    // * OUTPUT has been removed or added
    for (const auto &curr_miter : curr_rels) {
      const auto &id = curr_miter.first;
      if (prev_rels.find(id) == prev_rels.end()) {
        // MAP ADDED condition
        gen_selected_map.emplace(curr_miter.first,
                                 user_.rels_map.at(curr_miter.first));
        dirty_ = true;
      } else {
        // Check internal_inputs
        auto path_state = CheckPaths(prev_rels.at(id).internal_inputs,
                                     curr_miter.second.internal_inputs);
        auto changed =
            CheckChanged(prev_rels.at(id).outputs, curr_miter.second.outputs);

        if (path_state != internal::PathState::kNoChange || changed) {
          // MAP UPDATED condition
          gen_selected_map.emplace(curr_miter.first,
                                   user_.rels_map.at(curr_miter.first));
          dirty_ = true;
        } else {
          // MAP NO CHANGE condition
          dummy_gen_selected_map.emplace(curr_miter.first,
                                         user_.rels_map.at(curr_miter.first));
        }
      }
    }
  }
}

void CustomGenerator::AddSuccessSchema(const std::string &id,
                                       const UserRelInputOutputSchema &schema) {
  std::lock_guard<std::mutex> guard(success_schema_mutex_);
  success_schema_.emplace(id, schema);
}

void CustomGenerator::GenerateTask() {
  tf::Task generate_task = tf_.emplace([&](tf::Subflow &subflow) {
    if (env::get_task_state() != env::TaskState::SUCCESS) {
      return;
    }

    try {
      Convert();
      BuildGenerate(selected_user_schema_, dummy_selected_user_schema_);

      if (dirty_) {
        std::unordered_map<std::string, tf::Task> task_map;

        for (const auto &selected_miter : selected_user_schema_) {
          const auto &id = selected_miter.first;
          const auto &info = selected_miter.second;

          tf::Task task = subflow.emplace([&]() {
            try {
              CustomGeneratorContext ctx(command_, info.inputs, info.outputs);
              bool success = info.generate_cb(ctx);
              env::assert_fatal(success, "Generate Cb failed for id {}");
              AddSuccessSchema(id, info);
            } catch (...) {
              env::set_task_state(env::TaskState::FAILURE);
            }
          });
          task.name(id);
          task_map.emplace(id, task);
        }

        if (dependency_cb_) {
          dependency_cb_(task_map);
        }

        // TODO, Create Selected graph
      }

      // TODO, Create Dummy Selected graph

    } catch (...) {
      env::set_task_state(env::TaskState::FAILURE);
    }
  });
  generate_task.name(kGenerateTaskName);

  tf::Task end_task = tf_.emplace([this]() {
    // Even if env::TaskState::FAILURE we still need to partially store the
    // built files
    if (dirty_) {
      try {
        UserCustomGeneratorSchema user_final_schema;
        user_final_schema.rels_map.insert(dummy_selected_user_schema_.begin(),
                                          dummy_selected_user_schema_.end());
        user_final_schema.rels_map.insert(success_schema_.begin(),
                                          success_schema_.end());

        user_final_schema.ConvertToInternal();
        serialization_.UpdateStore(user_final_schema);
        env::assert_fatal(serialization_.StoreToFile(),
                          fmt::format("Store failed for {}", name_));
      } catch (...) {
        env::set_task_state(env::TaskState::FAILURE);
      }
    }
  });
  end_task.name(kEndGeneratorTaskName);

  // Dependencies
  generate_task.precede(end_task);
}

} // namespace buildcc
