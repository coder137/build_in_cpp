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

#include "taskflow/taskflow.hpp"

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
                         const fs_unordered_set &o)
      : command(c), inputs(i), outputs(o) {}

public:
  const env::Command &command;
  const fs_unordered_set &inputs;
  const fs_unordered_set &outputs;
};

// clang-format off
typedef std::function<bool(CustomGeneratorContext &)> GenerateCb;

typedef std::function<void(std::unordered_map<std::string, tf::Task> &)> DependencyCb;
// clang-format on

class CustomBlobHandler {
public:
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

  std::vector<uint8_t> GetSerializedData() {
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

struct UserGenInfo : internal::GenInfo {
  fs_unordered_set inputs;
  GenerateCb generate_cb;
  std::shared_ptr<CustomBlobHandler> blob_handler{nullptr};
};

struct UserCustomGeneratorSchema : public internal::CustomGeneratorSchema {
  std::unordered_map<std::string, UserGenInfo> gen_info_map;

  void ConvertToInternal() {
    for (auto &r_miter : gen_info_map) {
      r_miter.second.internal_inputs = path_schema_convert(
          r_miter.second.inputs, internal::Path::CreateExistingPath);
      auto p = internal_gen_info_map.emplace(r_miter.first, r_miter.second);
      env::assert_fatal(p.second,
                        fmt::format("Could not save {}", r_miter.first));
    }
  }
};

class CustomGenerator : public internal::BuilderInterface {
public:
  CustomGenerator(const std::string &name, const TargetEnv &env)
      : name_(name),
        env_(env.GetTargetRootDir(), env.GetTargetBuildDir() / name),
        serialization_(env_.GetTargetBuildDir() / fmt::format("{}.bin", name)) {
    Initialize();
  }
  virtual ~CustomGenerator() = default;
  CustomGenerator(const CustomGenerator &) = delete;

  // From env::Command module, forwarding here
  void AddDefaultArgument(const std::string &identifier,
                          const std::string &pattern);
  void AddDefaultArguments(
      const std::unordered_map<std::string, std::string> &arguments);
  const std::string &Get(const std::string &file_identifier) const;

  /**
   * @brief Single Generator task for inputs->generate_cb->outputs
   *
   * @param id Unique id associated with Generator task
   * @param inputs File inputs
   * @param outputs File outputs
   * @param generate_cb User-defined generate callback to build outputs from the
   * provided inputs
   */
  void AddGenInfo(const std::string &id, const fs_unordered_set &inputs,
                  const fs_unordered_set &outputs,
                  const GenerateCb &generate_cb,
                  std::shared_ptr<CustomBlobHandler> blob_handler = nullptr);

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
  void AddDependencyCb(const DependencyCb &dependency_cb);

  void Build() override;

  // Getters
  const std::string &GetName() const { return name_; }
  const fs::path &GetBinaryPath() const {
    return serialization_.GetSerializedFile();
  }
  const fs::path &GetRootDir() const { return env_.GetTargetRootDir(); }
  const fs::path &GetBuildDir() const { return env_.GetTargetBuildDir(); }
  tf::Taskflow &GetTaskflow() { return tf_; }

private:
  void Initialize();

  template <bool run> void TaskRunner(const std::string &id);

  void GenerateTask();
  void BuildGenerate(
      std::unordered_map<std::string, UserGenInfo> &gen_selected_map,
      std::unordered_map<std::string, UserGenInfo> &dummy_gen_selected_map);

  // Recheck states
  void IdRemoved();
  void IdAdded();
  void IdUpdated();

protected:
  env::Command command_;

private:
  std::string name_;
  TargetEnv env_;
  internal::CustomGeneratorSerialization serialization_;

  // Serialization
  UserCustomGeneratorSchema user_;

  std::mutex success_schema_mutex_;
  std::unordered_map<std::string, UserGenInfo> success_schema_;

  // Internal
  tf::Taskflow tf_;

  // Callbacks
  DependencyCb dependency_cb_;
};

} // namespace buildcc

#endif
