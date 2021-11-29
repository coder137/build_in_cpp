/*
 * Copyright 2021 Niket Naidu. All rights reserved.
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

#include "command/command.h"

#include "target/interface/builder_interface.h"

#include "target/base/generator_loader.h"
#include "target/common/path.h"

namespace buildcc::base {

class Generator : public BuilderInterface {
public:
  Generator(const std::string &name,
            const fs::path &target_path_relative_to_root, bool parallel = false)
      : name_(name), generator_root_dir_(env::get_project_root_dir() /
                                         target_path_relative_to_root),
        generator_build_dir_(env::get_project_build_dir() / name),
        loader_(name, generator_build_dir_), parallel_(parallel) {
    Initialize();
  }
  Generator(const Generator &generator) = delete;

  /**
   * @brief Add default arguments for input, output and command requirements
   *
   * @param arguments Key-Value pair for arguments
   * NOTE, Key must be a variable (lvalue) not a constant (rvalue)
   *
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
   * NOTE: The order of all commands are maintained (`std::vector::push_back`)
   *
   * If you would like to run the commands in parallel, set parallel == true in
   * the constructor
   */
  void AddCommand(
      const std::string &command_pattern,
      const std::unordered_map<const char *, std::string> &arguments = {});

  void Build() override;

  // Getter
  const fs::path &GetBinaryPath() const { return loader_.GetBinaryPath(); }
  tf::Taskflow &GetTaskflow() { return tf_; }

  const std::string &GetName() { return name_; }
  env::TaskState GetTaskState() { return task_state_; }

  const std::string &
  GetValueByIdentifier(const std::string &file_identifier) const;

private:
  void Initialize();

  void GenerateTask();
  void Convert();
  void BuildGenerate();

  bool Store() override;

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
  internal::GeneratorLoader loader_;

  // Serialization
  internal::RelationalPathFiles current_input_files_;
  internal::fs_unordered_set current_output_files_;
  std::vector<std::string> current_commands_;
  bool parallel_{false};

  // Internal
  std::mutex task_state_mutex_;
  env::TaskState task_state_{env::TaskState::SUCCESS};
  Command command_;
  tf::Taskflow tf_;
};

} // namespace buildcc::base

namespace buildcc {

typedef base::Generator BaseGenerator;

}

#endif
