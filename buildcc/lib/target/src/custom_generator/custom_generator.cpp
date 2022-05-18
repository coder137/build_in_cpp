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

} // namespace

namespace buildcc {

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

const std::string &
CustomGenerator::Get(const std::string &file_identifier) const {
  return command_.GetDefaultValueByKey(file_identifier);
}

void CustomGenerator::AddGenInfo(
    const std::string &id, const fs_unordered_set &inputs,
    const fs_unordered_set &outputs, const GenerateCb &generate_cb,
    std::shared_ptr<CustomBlobHandler> blob_handler) {
  env::assert_fatal(user_.gen_info_map.find(id) == user_.gen_info_map.end(),
                    fmt::format("Duplicate id {} detected", id));
  ASSERT_FATAL(generate_cb, "Invalid callback provided");

  UserGenInfo schema;
  for (const auto &i : inputs) {
    fs::path input =
        internal::Path::CreateNewPath(command_.Construct(path_as_string(i)))
            .GetPathname();
    schema.inputs.emplace(std::move(input));
  }
  for (const auto &o : outputs) {
    fs::path output =
        internal::Path::CreateNewPath(command_.Construct(path_as_string(o)))
            .GetPathname();
    schema.outputs.emplace(std::move(output));
  }
  schema.generate_cb = generate_cb;
  schema.blob_handler = std::move(blob_handler);
  user_.gen_info_map.try_emplace(id, std::move(schema));
  ungrouped_ids_.emplace(id);
}

void CustomGenerator::AddGroup(const std::string &group_id,
                               std::initializer_list<std::string> ids,
                               const DependencyCb &dependency_cb) {
  // Verify that the ids exist
  // Remove those ids from ungrouped_ids
  for (const auto &id : ids) {
    env::assert_fatal(user_.gen_info_map.find(id) != user_.gen_info_map.end(),
                      fmt::format("Id '{}' is not found", id));
    ungrouped_ids_.erase(id);
  }

  env::assert_fatal(grouped_ids_.find(group_id) == grouped_ids_.end(),
                    fmt::format("Group Id '{}' duplicate found", group_id));

  // Group map is used to group similar ids in a single subflow
  GroupMetadata group_metadata;
  group_metadata.ids = ids;
  group_metadata.dependency_cb = dependency_cb;
  grouped_ids_.try_emplace(group_id, std::move(group_metadata));
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
      {"project_root_dir", path_as_string(Project::GetRootDir())},
      {"project_build_dir", path_as_string(Project::GetBuildDir())},
      {"gen_root_dir", path_as_string(env_.GetTargetRootDir())},
      {"gen_build_dir", path_as_string(env_.GetTargetBuildDir())},
  });

  //
  unique_id_ = name_;
  tf_.name(name_);
}

void CustomGenerator::BuildGenerate(
    std::unordered_set<std::string> &gen_selected_ids,
    std::unordered_set<std::string> &dummy_gen_selected_ids) {
  if (!serialization_.IsLoaded()) {
    std::for_each(
        user_.gen_info_map.begin(), user_.gen_info_map.end(),
        [&](const auto &iter) { gen_selected_ids.insert(iter.first); });
    dirty_ = true;
  } else {
    // DONE, Conditionally select internal_gen_info_map depending on what has
    // changed
    const auto &prev_gen_info_map =
        serialization_.GetLoad().internal_gen_info_map;
    const auto &curr_gen_info_map = user_.gen_info_map;

    // DONE, MAP REMOVED condition Check if prev_gen_info_map exists in
    // curr_gen_info_map If prev_gen_info_map does not exist in
    // curr_gen_info_map, has been removed from existing build We need this
    // condition to only set the dirty_ flag
    for (const auto &[id, _] : prev_gen_info_map) {
      if (curr_gen_info_map.find(id) == curr_gen_info_map.end()) {
        // MAP REMOVED condition
        IdRemoved();
        dirty_ = true;
        break;
      }
    }

    // DONE, MAP ADDED condition Check if curr_gen_info_map exists in
    // prev_gen_info_map If curr_gen_info_map does not exist in
    // prev_gen_info_map, has been added to existing build
    for (const auto &[id, _] : curr_gen_info_map) {
      if (prev_gen_info_map.find(id) == prev_gen_info_map.end()) {
        // MAP ADDED condition
        IdAdded();
        gen_selected_ids.insert(id);
        dirty_ = true;
      } else {
        // MAP UPDATED condition (*checked later)
        // This is because tasks can have dependencies amongst each other we can
        // compute task level rebuilds later
        dummy_gen_selected_ids.insert(id);
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
      std::unordered_map<std::string, tf::Task> registered_tasks;

      // Selected ids for build
      std::unordered_set<std::string> selected_ids;
      std::unordered_set<std::string> dummy_selected_ids;
      BuildGenerate(selected_ids, dummy_selected_ids);

      // Grouped tasks
      for (const auto &[first, second] : grouped_ids_) {
        const auto &group_id = first;
        const auto &group_metadata = second;
        auto group_task = subflow.emplace([&](tf::Subflow &s) {
          std::unordered_map<std::string, tf::Task> reg_tasks;

          if (env::get_task_state() != env::TaskState::SUCCESS) {
            return;
          }

          for (const auto &id : group_metadata.ids) {
            bool build = selected_ids.count(id) == 1;
            auto task = CreateTaskRunner(s, build, id);
            task.name(id);
            reg_tasks.try_emplace(id, task);
          }

          // Dependency callback
          group_metadata.InvokeDependencyCb(group_id, std::move(reg_tasks));

          // NOTE, Do not call detach otherwise this will fail
          s.join();
        });
        group_task.name(group_id);
        registered_tasks.try_emplace(group_id, group_task);
      }

      // Ungrouped tasks
      for (const auto &id : ungrouped_ids_) {
        bool build = selected_ids.count(id) == 1;
        auto task = CreateTaskRunner(subflow, build, id);
        task.name(id);
        registered_tasks.try_emplace(id, task);
      }

      // Dependencies between tasks
      InvokeDependencyCb(std::move(registered_tasks));

      // NOTE, Do not call detach otherwise this will fail
      subflow.join();

      // Store dummy_selected and successfully run schema
      if (dirty_) {
        UserCustomGeneratorSchema user_final_schema;
        user_final_schema.gen_info_map.insert(success_schema_.begin(),
                                              success_schema_.end());

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

void CustomGenerator::InvokeDependencyCb(
    std::unordered_map<std::string, tf::Task> &&registered_tasks)
    const noexcept {
  if (dependency_cb_) {
    try {
      dependency_cb_(std::move(registered_tasks));
    } catch (...) {
      env::log_critical(__FUNCTION__, "Dependency callback failed");
      env::set_task_state(env::TaskState::FAILURE);
    }
  }
}

tf::Task CustomGenerator::CreateTaskRunner(tf::Subflow &subflow, bool build,
                                           const std::string &id) {
  return subflow.emplace([&, build, id]() {
    if (env::get_task_state() != env::TaskState::SUCCESS) {
      return;
    }
    try {
      TaskRunner(build, id);
    } catch (...) {
      env::set_task_state(env::TaskState::FAILURE);
    }
  });
}

void CustomGenerator::TaskRunner(bool run, const std::string &id) {
  // Convert
  auto &current_gen_info = user_.gen_info_map.at(id);
  current_gen_info.internal_inputs = internal::path_schema_convert(
      current_gen_info.inputs, internal::Path::CreateExistingPath);
  current_gen_info.userblob =
      current_gen_info.blob_handler != nullptr
          ? current_gen_info.blob_handler->GetSerializedData()
          : std::vector<uint8_t>();

  // Run
  const auto &current_info = user_.gen_info_map.at(id);
  bool rerun = false;
  if (run) {
    rerun = true;
  } else {
    const auto &previous_info =
        serialization_.GetLoad().internal_gen_info_map.at(id);
    rerun = internal::CheckPaths(previous_info.internal_inputs,
                                 current_info.internal_inputs) !=
                internal::PathState::kNoChange ||
            internal::CheckChanged(previous_info.outputs, current_info.outputs);
    if (!rerun && current_info.blob_handler != nullptr) {
      rerun = current_info.blob_handler->CheckChanged(previous_info.userblob,
                                                      current_info.userblob);
    }
  }

  if (rerun) {
    dirty_ = true;
    buildcc::CustomGeneratorContext ctx(command_, current_info.inputs,
                                        current_info.outputs,
                                        current_info.userblob);
    bool success = current_info.generate_cb(ctx);
    env::assert_fatal(success, fmt::format("Generate Cb failed for id {}", id));
  }

  std::scoped_lock<std::mutex> guard(success_schema_mutex_);
  success_schema_.try_emplace(id, current_info);
}

} // namespace buildcc
