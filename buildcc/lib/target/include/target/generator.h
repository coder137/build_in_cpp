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

typedef std::function<bool(
    const internal::geninfo_unordered_map &previous_info,
    const internal::geninfo_unordered_map &current_info,
    std::vector<const internal::GenInfo *> &output_generated_files,
    std::vector<const internal::GenInfo *> &output_dummy_generated_files)>
    custom_regenerate_cb_params;

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
   * @brief Implement your own Regenerate callback.
   * - See `Regenerate` function for generic implementation
   * - See `Recheck*` APIs in BuilderInterface for custom checks
   *
   * @param cb Gives 4 parameters
   *
   * const internal::geninfo_unordered_map &previous_info
   * const internal::geninfo_unordered_map &current_info
   * std::vector<const internal::GenInfo *> &output_generated_files
   * std::vector<const internal::GenInfo *> &output_dummy_generated_files
   *
   * @return cb should return true or false which sets the dirty_ flag
   */
  void AddCustomRegenerateCb(const custom_regenerate_cb_params &cb);

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
  void Build(tf::FlowBuilder &builder);

  // Getter
  fs::path GetBinaryPath() const { return loader_.GetBinaryPath(); }
  tf::Taskflow &GetTaskflow() { return tf_; }

private:
  void GenerateTask(tf::FlowBuilder &builder);
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

  custom_regenerate_cb_params custom_regenerate_cb_;
  std::function<void(void)> pregenerate_cb_{[]() {}};
  std::function<void(void)> postgenerate_cb_{[]() {}};

  internal::GeneratorLoader loader_;

  tf::Taskflow tf_;
};

} // namespace buildcc::base

#endif
