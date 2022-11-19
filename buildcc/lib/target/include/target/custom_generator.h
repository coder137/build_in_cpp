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

#include "target/common/target_env.h"

namespace buildcc {

// TODO, Shift to a different file
// TODO, Check if we need the "id" here as well
class CustomGeneratorContext {
public:
  CustomGeneratorContext(const env::Command &c, const fs_unordered_set &i,
                         const fs_unordered_set &o,
                         const std::vector<uint8_t> &ub)
      : command(c), inputs(i), outputs(o), userblob(ub) {}

  const env::Command &command;
  const fs_unordered_set &inputs;
  const fs_unordered_set &outputs;
  const std::vector<uint8_t> &userblob;
};

// clang-format off
using GenerateCb = std::function<bool (CustomGeneratorContext &)>;

using DependencyCb = std::function<void (std::unordered_map<std::string, tf::Task> &&)>;
// clang-format on

class CustomBlobHandler {
public:
  CustomBlobHandler() = default;
  virtual ~CustomBlobHandler() = default;

  bool CheckChanged(const std::vector<uint8_t> &previous,
                    const std::vector<uint8_t> &current) const {
    env::assert_fatal(
        Verify(previous),
        "Stored blob is corrupted or User verification is incorrect");
    env::assert_fatal(
        Verify(current),
        "Current blob is corrupted or User verification is incorrect");
    return !IsEqual(previous, current);
  };

  std::vector<uint8_t> GetSerializedData() const {
    auto serialized_data = Serialize();
    env::assert_fatal(
        Verify(serialized_data),
        "Serialized data is corrupted or Serialize function is incorrect");
    return serialized_data;
  }

private:
  virtual bool Verify(const std::vector<uint8_t> &serialized_data) const = 0;
  virtual bool IsEqual(const std::vector<uint8_t> &previous,
                       const std::vector<uint8_t> &current) const = 0;
  virtual std::vector<uint8_t> Serialize() const = 0;
};

struct UserCustomGeneratorSchema : public internal::CustomGeneratorSchema {
  struct UserIdInfo : internal::CustomGeneratorSchema::IdInfo {
    fs_unordered_set inputs; // TODO, Remove
    GenerateCb generate_cb;
    std::shared_ptr<CustomBlobHandler> blob_handler{nullptr};
  };

  using UserIdPair = std::pair<const IdKey, UserIdInfo>;
  std::unordered_map<IdKey, UserIdInfo> ids;

  void ConvertToInternal() {
    for (auto &[id_key, id_info] : ids) {
      id_info.internal_inputs = path_schema_convert(
          id_info.inputs, internal::Path::CreateExistingPath);
      auto [_, success] = internal_ids.try_emplace(id_key, id_info);
      env::assert_fatal(success, fmt::format("Could not save {}", id_key));
    }
  }
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
  void AddIdInfo(const std::string &id,
                 const std::unordered_set<std::string> &inputs,
                 const std::unordered_set<std::string> &outputs,
                 const GenerateCb &generate_cb,
                 std::shared_ptr<CustomBlobHandler> blob_handler = nullptr);

  // TODO, Doc
  void AddGroupInfo(const std::string &group_id,
                    std::initializer_list<std::string> ids);

  // Callbacks
  /**
   * @brief Setup dependencies between Tasks using their `id`
   * For example: `task_map["id1"].precede(task_map["id2"])`
   *
   * IMPORTANT: Successor tasks will not automatically run if dependent task is
   * run.
   * The Dependency callback only sets precedence (order in which your tasks
   * should run)
   * Default behaviour when dependency callback is not supplied: All task `id`s
   * run in parallel.
   *
   * @param dependency_cb Unordered map of `id` and `task`
   * The map can be safely mutated.
   */
  // void AddDependencyCb(const DependencyCb &dependency_cb);

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
  struct GroupMetadata {
    std::vector<std::string> ids;
    // DependencyCb dependency_cb;
    // void InvokeDependencyCb(std::unordered_map<std::string, tf::Task>
    //                             &&registered_tasks) const noexcept {
    //   if (dependency_cb) {
    //     try {
    //       dependency_cb(std::move(registered_tasks));
    //     } catch (...) {
    //       // TODO, Put a logging message here
    //       env::set_task_state(env::TaskState::FAILURE);
    //     }
    //   }
    // }
  };

  struct Comparator {
    Comparator(const internal::CustomGeneratorSchema &s,
               const UserCustomGeneratorSchema &us)
        : schema(s), user_schema(us) {}

    enum class State {
      kRemoved,
      kAdded,
      kCheckLater,
    };

    void AddAllIds() {
      const auto &curr_ids = user_schema.ids;
      for (const auto &[id, _] : curr_ids) {
        id_state_info.at(State::kAdded).insert(id);
      }
    }

    void CompareIds() {
      const auto &prev_ids = schema.internal_ids;
      const auto &curr_ids = user_schema.ids;

      for (const auto &[prev_id, _] : prev_ids) {
        if (curr_ids.find(prev_id) == curr_ids.end()) {
          // Id Removed condition, previous id is not present in the current run
          id_state_info.at(State::kRemoved).insert(prev_id);
        }
      }

      for (const auto &[curr_id, _] : curr_ids) {
        if (prev_ids.find(curr_id) == prev_ids.end()) {
          // Id Added condition
          id_state_info.at(State::kAdded).insert(curr_id);
        } else {
          // Id Check Later condition
          id_state_info.at(State::kCheckLater).insert(curr_id);
        }
      }
    }

    void CompareGroups() {
      const auto &prev_groups = schema.internal_groups;
      const auto &curr_groups = user_schema.internal_groups;

      for (const auto &[prev_group_id, _] : prev_groups) {
        if (curr_groups.find(prev_group_id) == curr_groups.end()) {
          // Group Removed condition
          group_state_info.at(State::kRemoved).insert(prev_group_id);
        }
      }

      for (const auto &[curr_group_id, curr_group_info] : curr_groups) {
        if (prev_groups.find(curr_group_id) == prev_groups.end()) {
          // Group Added condition
          group_state_info.at(State::kAdded).insert(curr_group_id);
        } else {
          // Group Check Later condition
          // NOTE, We cannot perform the check now since the input/metadata
          // might be added during runtime (when task executes)
          group_state_info.at(State::kCheckLater).insert(curr_group_id);
        }
      }
    }

    const std::unordered_set<std::string> &RemovedIds() const {
      return id_state_info.at(State::kRemoved);
    }

    const std::unordered_set<std::string> &AddedIds() const {
      return id_state_info.at(State::kAdded);
    }

    bool AddedId(const std::string &id) const {
      return id_state_info.at(State::kAdded).count(id) == 1;
    }

    bool AddedGroupId(const std::string &group_id) const {
      return group_state_info.at(State::kAdded).count(group_id) == 1;
    }

  private:
    const internal::CustomGeneratorSchema &schema;
    const UserCustomGeneratorSchema &user_schema;
    std::unordered_map<State, std::unordered_set<std::string>> id_state_info{
        {State::kRemoved, std::unordered_set<std::string>()},
        {State::kAdded, std::unordered_set<std::string>()},
        {State::kCheckLater, std::unordered_set<std::string>()},
    };

    std::unordered_map<State, std::unordered_set<std::string>> group_state_info{
        {State::kRemoved, std::unordered_set<std::string>()},
        {State::kAdded, std::unordered_set<std::string>()},
        {State::kCheckLater, std::unordered_set<std::string>()},
    };
  };

private:
  void Initialize();

  tf::Task CreateGroupTask(tf::FlowBuilder &builder,
                           const GroupMetadata &group_metadata);

  tf::Task CreateTaskRunner(tf::Subflow &subflow, const std::string &id);
  void TaskRunner(const std::string &id);

  void GenerateTask();
  void BuildGenerate();

  // void InvokeDependencyCb(std::unordered_map<std::string, tf::Task>
  //                             &&registered_tasks) const noexcept;

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
  std::unordered_map<std::string, GroupMetadata> grouped_ids_;
  std::unordered_set<std::string> ungrouped_ids_;

  // Comparator
  Comparator comparator_;

  std::mutex success_schema_mutex_;
  std::unordered_map<std::string, UserCustomGeneratorSchema::UserIdInfo>
      success_schema_;

  // Internal
  env::Command command_;

  // // Callbacks
  // DependencyCb dependency_cb_;
};

} // namespace buildcc

#endif
