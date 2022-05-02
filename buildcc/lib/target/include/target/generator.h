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

#ifndef TARGET_GENERATOR_H_
#define TARGET_GENERATOR_H_

#include <functional>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "taskflow/taskflow.hpp"

#include "env/env.h"
#include "env/task_state.h"

#include "env/command.h"

#include "target/interface/builder_interface.h"

#include "schema/custom_generator_serialization.h"
#include "schema/generator_serialization.h"
#include "schema/path.h"

#include "target/common/target_env.h"

namespace buildcc {

struct UserRelInputOutputSchema {
  fs_unordered_set inputs;
  fs_unordered_set outputs;
};

struct UserCustomGeneratorSchema : public internal::CustomGeneratorSchema {
  std::unordered_map<std::string, UserRelInputOutputSchema> rels_map;

  void ConvertToInternal() {
    for (const auto &r_miter : rels_map) {
      internal::RelInputOutputSchema internal_schema;
      internal_schema.outputs = r_miter.second.outputs;
      internal_schema.internal_inputs = internal::path_schema_convert(
          r_miter.second.inputs, internal::Path::CreateExistingPath);
      internal_rels_map.emplace(r_miter.first, std::move(internal_schema));
    }
  }
};

class CustomGeneratorContext {
public:
  CustomGeneratorContext(const env::Command &c) : command(c) {}

  void Success(const std::string &id) {
    std::lock_guard<std::mutex> m(success_schema_mutex_);
    success_schema_.emplace(id, selected_user_schema.at(id));
  }

  void Failure() { env::set_task_state(env::TaskState::FAILURE); }

  const std::unordered_map<std::string, UserRelInputOutputSchema> &
  GetSuccessSchema() {
    return success_schema_;
  }

public:
  const env::Command &command;
  std::unordered_map<std::string, UserRelInputOutputSchema>
      selected_user_schema;

private:
  std::mutex success_schema_mutex_;
  std::unordered_map<std::string, UserRelInputOutputSchema> success_schema_;
};

// clang-format off
typedef std::function<std::unordered_map<std::string, tf::Task>(tf::Subflow &, CustomGeneratorContext &)> GenerateCb;
// clang-format on

class CustomGenerator : public internal::BuilderInterface {
public:
  CustomGenerator(const std::string &name, const TargetEnv &env,
                  bool parallel = false)
      : name_(name),
        env_(env.GetTargetRootDir(), env.GetTargetBuildDir() / name),
        serialization_(env_.GetTargetBuildDir() / fmt::format("{}.bin", name)),
        parallel_(parallel), ctx_(command_) {
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

  void AddRelInputOutput(const std::string &id, const fs_unordered_set &inputs,
                         const fs_unordered_set &outputs);

  // TODO, Rename this to GenerateCb
  void AddGenerateCb(const GenerateCb &regenerate_cb);

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
  // Recheck states
  void PathRemoved() {}
  void PathAdded() {}
  void PathUpdated() {}

  void StringChanged() {}

private:
  std::string name_;
  TargetEnv env_;
  internal::CustomGeneratorSerialization serialization_;
  bool parallel_;
  CustomGeneratorContext ctx_;

  // Serialization
  UserCustomGeneratorSchema user_;
  std::unordered_map<std::string, UserRelInputOutputSchema>
      dummy_selected_user_schema_;

  // Internal
  std::mutex task_state_mutex_;
  env::Command command_;
  tf::Taskflow tf_;
  GenerateCb regenerate_cb_;
};

// TODO, Make this private
struct UserGeneratorSchema : public internal::GeneratorSchema {
  fs_unordered_set inputs;
};

class Generator : public internal::BuilderInterface {
public:
  Generator(const std::string &name, const TargetEnv &env,
            bool parallel = false)
      : name_(name), generator_root_dir_(env.GetTargetRootDir()),
        generator_build_dir_(env.GetTargetBuildDir() / name),
        serialization_(generator_build_dir_ / fmt::format("{}.bin", name)),
        parallel_(parallel) {
    Initialize();
  }
  virtual ~Generator() = default;
  Generator(const Generator &) = delete;

  /**
   * @brief Add default arguments for input, output and command requirements
   *
   * @param arguments Key-Value pair for arguments
   */
  void AddDefaultArguments(
      const std::unordered_map<std::string, std::string> &arguments);

  /**
   * @brief Add absolute input path pattern to generator
   * NOTE: We can use {gen_root_dir} and {gen_build_dir} in the
   * absolute_input_pattern
   *
   * If `identifier` is supplied it is added to default arguments as a key
   * Example: fmt::format("{identifier}") -> "absolute_input_pattern"
   */
  void AddInput(const std::string &absolute_input_pattern,
                const char *identifier = nullptr);

  /**
   * @brief Add absolute output path pattern to generator
   * NOTE: We can use {gen_root_dir} and {gen_build_dir} in the
   * absolute_output_pattern
   *
   * If `identifier` is supplied it is added to default arguments as a key
   * Example: fmt::format("{identifier}") -> "absolute_output_pattern"
   */
  void AddOutput(const std::string &absolute_output_pattern,
                 const char *identifier = nullptr);

  /**
   * @brief Add a command_pattern that is fed to `Command::Execute` internally
   *
   * NOTE: The order of all commands are maintained (`std::vector::push_back`)
   *
   * If you would like to run the commands in parallel, set `parallel == true`
   * in the constructor
   */
  void AddCommand(
      const std::string &command_pattern,
      const std::unordered_map<const char *, std::string> &arguments = {});

  /**
   * @brief Build Generator Tasks
   *
   * Use `GetTaskflow` for the registered tasks
   */
  void Build() override;

  // Getter
  const fs::path &GetBinaryPath() const {
    return serialization_.GetSerializedFile();
  }
  tf::Taskflow &GetTaskflow() { return tf_; }

  const std::string &GetName() const { return name_; }
  env::TaskState GetTaskState() const { return task_state_; }

  const std::string &
  GetValueByIdentifier(const std::string &file_identifier) const;

private:
  void Initialize();

  void GenerateTask();
  void Convert();
  void BuildGenerate();

  // Recheck states
  void InputRemoved();
  void InputAdded();
  void InputUpdated();

  void OutputChanged();
  void CommandChanged();

private:
  // Constructor
  std::string name_;
  fs::path generator_root_dir_;
  fs::path generator_build_dir_;
  internal::GeneratorSerialization serialization_;
  bool parallel_{false};

  // Serialization
  UserGeneratorSchema user_;

  // Internal
  std::mutex task_state_mutex_;
  env::TaskState task_state_{env::TaskState::SUCCESS};
  env::Command command_;
  tf::Taskflow tf_;
};

typedef Generator BaseGenerator;

} // namespace buildcc

#endif
