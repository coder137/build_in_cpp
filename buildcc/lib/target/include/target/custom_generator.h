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

#ifndef TARGET_CUSTOM_GENERATOR_H_
#define TARGET_CUSTOM_GENERATOR_H_

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "env/command.h"
#include "env/task_state.h"

#include "target/interface/builder_interface.h"

#include "schema/custom_generator_serialization.h"
#include "schema/path.h"

#include "custom_generator/custom_blob_handler.h"
#include "custom_generator/custom_generator_context.h"

#include "target/common/target_env.h"

namespace buildcc {

struct UserCustomGeneratorSchema : public internal::CustomGeneratorSchema {
  struct UserIdInfo : internal::CustomGeneratorSchema::IdInfo {
    GenerateCb generate_cb;
    std::shared_ptr<CustomBlobHandler> blob_handler{nullptr};

    void ConvertToInternal() {
      inputs.ComputeHashForAll();
      userblob = blob_handler != nullptr ? blob_handler->GetSerializedData()
                                         : std::vector<uint8_t>();
    }
  };

  std::unordered_map<IdKey, UserIdInfo> ids;

  void ConvertToInternal() {
    for (auto &[id_key, id_info] : ids) {
      id_info.ConvertToInternal();
      auto [_, success] = internal_ids.try_emplace(id_key, id_info);
      env::assert_fatal(success, fmt::format("Could not save {}", id_key));
    }
  }
};

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

  void CompareIds() {
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

class CustomGenerator : public internal::BuilderInterface {
public:
  CustomGenerator(const std::string &name, const TargetEnv &env)
      : name_(name),
        env_(env.GetTargetRootDir(), env.GetTargetBuildDir() / name),
        serialization_(env_.GetTargetBuildDir() / fmt::format("{}.json", name)),
        comparator_(serialization_.GetLoad(), user_) {
    Initialize();
  }
  virtual ~CustomGenerator() = default;
  CustomGenerator(const CustomGenerator &) = delete;

  // TODO, Doc
  void AddPattern(const std::string &identifier, const std::string &pattern);

  // TODO, Doc
  void
  AddPatterns(const std::unordered_map<std::string, std::string> &pattern_map);

  // TODO, Doc
  std::string ParsePattern(const std::string &pattern,
                           const std::unordered_map<const char *, std::string>
                               &arguments = {}) const;

  /**
   * @brief Single Generator task for inputs->generate_cb->outputs
   *
   * @param id Unique id associated with Generator task
   * @param inputs File inputs
   * @param outputs File outputs
   * @param generate_cb User-defined generate callback to build outputs from the
   * provided inputs
   */
  void
  AddIdInfo(const std::string &id,
            const std::unordered_set<std::string> &inputs,
            const std::unordered_set<std::string> &outputs,
            const GenerateCb &generate_cb,
            const std::shared_ptr<CustomBlobHandler> &blob_handler = nullptr);

  void Build() override;

  // Getters
  const std::string &GetName() const { return name_; }
  const fs::path &GetBinaryPath() const {
    return serialization_.GetSerializedFile();
  }
  const fs::path &GetRootDir() const { return env_.GetTargetRootDir(); }
  const fs::path &GetBuildDir() const { return env_.GetTargetBuildDir(); }
  const std::string &Get(const std::string &file_identifier) const;

private:
  void Initialize();

  tf::Task CreateTaskRunner(tf::Subflow &subflow, const std::string &id);
  void TaskRunner(const std::string &id);

  void GenerateTask();
  void BuildGenerate();

  // Recheck states
  void IdRemoved();
  void IdAdded();
  void IdUpdated();

protected:
  const env::Command &ConstCommand() const { return command_; }
  env::Command &RefCommand() { return command_; }

private:
  std::string name_;
  TargetEnv env_;
  internal::CustomGeneratorSerialization serialization_;

  // Serialization
  UserCustomGeneratorSchema user_;

  // Internal
  env::Command command_;

  // Comparator
  // TODO, Shift internally
  Comparator comparator_;

  // TODO, Shift internally
  std::mutex success_schema_mutex_;
  std::unordered_map<std::string, UserCustomGeneratorSchema::UserIdInfo>
      success_schema_;
};

} // namespace buildcc

#endif
