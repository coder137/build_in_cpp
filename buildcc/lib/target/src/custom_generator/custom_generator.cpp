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
    std::shared_ptr<CustomBlobHandler> blob_handler) {
  env::assert_fatal(user_.ids.find(id) == user_.ids.end(),
                    fmt::format("Duplicate id {} detected", id));
  ASSERT_FATAL(generate_cb, "Invalid callback provided");

  UserCustomGeneratorSchema::UserIdInfo schema;
  for (const auto &i : inputs) {
    fs::path input = string_as_path(command_.Construct(i));
    schema.inputs.emplace(std::move(input));
  }
  for (const auto &o : outputs) {
    fs::path output = string_as_path(command_.Construct(o));
    schema.outputs.emplace(std::move(output));
  }
  schema.generate_cb = generate_cb;
  schema.blob_handler = std::move(blob_handler);

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

/**
 * @brief Check which ids need to be rebuilt
 *
 * @param gen_selected_ids
 * @param dummy_gen_selected_ids
 */
void CustomGenerator::BuildGenerate() {
  if (!serialization_.IsLoaded()) {
    comparator_.AddAllIds();
    dirty_ = true;
  } else {
    // For IDS
    comparator_.CompareIds();

    const bool is_removed = !comparator_.GetRemovedIds().empty();
    const bool is_added = !comparator_.GetAddedIds().empty();
    dirty_ = is_removed || is_added;

    if (is_removed) {
      IdRemoved();
    }

    for (const auto &id : comparator_.GetAddedIds()) {
      (void)id;
      IdAdded();
    }
  }
}

void CustomGenerator::GenerateTask() {
  tf::Task generate_task = tf_.emplace([&](tf::Subflow &subflow) {
    if (env::get_task_state() != env::TaskState::SUCCESS) {
      return;
    }

    try {
      // Selected ids for build
      BuildGenerate();

      // Create runner for each added/updated id
      for (const auto &id : comparator_.GetAddedIds()) {
        auto task = CreateTaskRunner(subflow, id);
        task.name(id);
      }

      for (const auto &id : comparator_.GetCheckLaterIds()) {
        auto task = CreateTaskRunner(subflow, id);
        task.name(id);
      }

      // NOTE, Do not call detach otherwise this will fail
      subflow.join();

      // Store
      if (dirty_) {
        UserCustomGeneratorSchema user_final_schema;
        user_final_schema.ids.insert(success_schema_.begin(),
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

tf::Task CustomGenerator::CreateTaskRunner(tf::Subflow &subflow,
                                           const std::string &id) {
  return subflow.emplace([&, id]() {
    if (env::get_task_state() != env::TaskState::SUCCESS) {
      return;
    }
    try {
      TaskRunner(id);
    } catch (...) {
      env::set_task_state(env::TaskState::FAILURE);
    }
  });
}

void CustomGenerator::TaskRunner(const std::string &id) {
  // Convert
  {
    auto &current_id_info = user_.ids.at(id);
    current_id_info.internal_inputs = internal::path_schema_convert(
        current_id_info.inputs, internal::Path::CreateExistingPath);
    current_id_info.userblob =
        current_id_info.blob_handler != nullptr
            ? current_id_info.blob_handler->GetSerializedData()
            : std::vector<uint8_t>();
  }

  // Compute runnable
  bool run = comparator_.IsIdAdded(id) ? true : comparator_.IsChanged(id);

  const auto &current_id_info = user_.ids.at(id);
  if (run) {
    dirty_ = true;
    CustomGeneratorContext ctx(command_, current_id_info.inputs,
                               current_id_info.outputs,
                               current_id_info.userblob);
    bool success = current_id_info.generate_cb(ctx);
    env::assert_fatal(success, fmt::format("Generate Cb failed for id {}", id));
  }

  std::scoped_lock<std::mutex> guard(success_schema_mutex_);
  success_schema_.try_emplace(id, current_id_info);
}

} // namespace buildcc
