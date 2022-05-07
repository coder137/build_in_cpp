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

struct UserRelInputOutputSchema : internal::RelInputOutputSchema {
  fs_unordered_set inputs;
  GenerateCb generate_cb;
};

struct UserCustomGeneratorSchema : public internal::CustomGeneratorSchema {
  std::unordered_map<std::string, UserRelInputOutputSchema> rels_map;

  void ConvertToInternal() {
    for (auto &r_miter : rels_map) {
      r_miter.second.internal_inputs = path_schema_convert(
          r_miter.second.inputs, internal::Path::CreateExistingPath);
      auto p = internal_rels_map.emplace(r_miter.first, r_miter.second);
      env::assert_fatal(p.second,
                        fmt::format("Could not save {}", r_miter.first));
    }
  }
};

// TODO, Add comments to functions
class CustomGenerator : public internal::BuilderInterface {
public:
  CustomGenerator(const std::string &name, const TargetEnv &env,
                  bool parallel = false)
      : name_(name),
        env_(env.GetTargetRootDir(), env.GetTargetBuildDir() / name),
        serialization_(env_.GetTargetBuildDir() / fmt::format("{}.bin", name)),
        parallel_(parallel) {
    Initialize();
  }
  virtual ~CustomGenerator() = default;
  CustomGenerator(const CustomGenerator &) = delete;

  /**
   * @brief Add default arguments for input, output and command requirements
   *
   * @param arguments Key-Value pair for arguments
   */
  void AddDefaultArguments(
      const std::unordered_map<std::string, std::string> &arguments);

  void AddGenInfo(const std::string &id, const fs_unordered_set &inputs,
                  const fs_unordered_set &outputs,
                  const GenerateCb &generate_cb);

  // Callbacks
  void AddDependencyCb(const DependencyCb &dependency_cb);

  void Build() override;

  // Getters
  const fs::path &GetBinaryPath() const {
    return serialization_.GetSerializedFile();
  }
  tf::Taskflow &GetTaskflow() { return tf_; }

private:
  void Initialize();

  void GenerateTask();
  void Convert();
  void BuildGenerate(std::unordered_map<std::string, UserRelInputOutputSchema>
                         &gen_selected_map,
                     std::unordered_map<std::string, UserRelInputOutputSchema>
                         &dummy_gen_selected_map);

  void AddSuccessSchema(const std::string &id,
                        const UserRelInputOutputSchema &schema);

  // Recheck states
  void IdRemoved();
  void IdAdded();
  void IdUpdated();

private:
  std::string name_;
  TargetEnv env_;
  internal::CustomGeneratorSerialization serialization_;
  bool parallel_;

  // Serialization
  UserCustomGeneratorSchema user_;

  std::mutex success_schema_mutex_;
  std::unordered_map<std::string, UserRelInputOutputSchema> success_schema_;

  // Internal
  std::mutex task_state_mutex_;
  env::Command command_;
  tf::Taskflow tf_;

  // Callbacks
  DependencyCb dependency_cb_;
};

} // namespace buildcc

#endif
