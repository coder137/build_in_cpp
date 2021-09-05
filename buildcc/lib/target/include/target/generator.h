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
#include <string>
#include <vector>

#include "taskflow/taskflow.hpp"

#include "env/env.h"

#include "target/builder_interface.h"

#include "target/generator_loader.h"
#include "target/path.h"

namespace buildcc::base {

class Generator : public BuilderInterface {
public:
  Generator(const std::string &name, const fs::path &path)
      : name_(name), loader_(name, path) {}
  Generator(const Generator &generator) = delete;

  /**
   * @brief Define your input - output - command relation
   *
   * @param name GenInfo task name
   * @param inputs Input files
   * @param outputs Output files generated
   * @param commands Commands for input files to generate output files
   * @param parallel Run commands in parallel
   */
  void AddGenInfo(const std::string &name,
                  const internal::fs_unordered_set &inputs,
                  const internal::fs_unordered_set &outputs,
                  const std::vector<std::string> &commands, bool parallel);

  /**
   * @brief Custom regenerate callback checked for every GenInfo
   *
   * @param cb
   */
  void AddRegenerateCb(const std::function<bool(void)> &cb);

  /**
   * @brief Custom pre generate callback run before the core generate function
   *
   * @param cb
   */
  void AddPregenerateCb(const std::function<void(void)> &cb);

  /**
   * @brief Custom post generate callback run after the core generate function
   * IF dirty_ == true
   *
   * @param cb
   */
  void AddPostgenerateCb(const std::function<void(void)> &cb);

  void Build() override;

  // Getter
  fs::path GetBinaryPath() const { return loader_.GetBinaryPath(); }
  tf::Taskflow &GetTaskflow() { return tf_; }

private:
  void GenerateTask();
  // Convert from UserGenInfo to internal::GenInfo
  void Convert();
  void
  BuildGenerate(std::vector<const internal::GenInfo *> &generated_files,
                std::vector<const internal::GenInfo *> &dummy_generated_files);
  bool
  Regenerate(std::vector<const internal::GenInfo *> &generated_files,
             std::vector<const internal::GenInfo *> &dummy_generated_files);

  bool Store() override;

  // Recheck states
  void InputRemoved();
  void InputAdded();
  void InputUpdated();

  void OutputChanged();
  void CommandChanged();

private:
  std::string name_;
  std::unordered_map<std::string, internal::GenInfo> current_info_;
  std::function<bool(void)> regenerate_cb_{[]() { return false; }};
  std::function<void(void)> pregenerate_cb_{[]() {}};
  std::function<void(void)> postgenerate_cb_{[]() {}};

  internal::GeneratorLoader loader_;

  tf::Taskflow tf_;
  tf::Task build_task_;
};

} // namespace buildcc::base

#endif
