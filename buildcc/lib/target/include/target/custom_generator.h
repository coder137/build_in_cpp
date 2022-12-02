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
    void ConvertToInternal() {
      inputs.ComputeHashForAll();
      userblob = blob_handler != nullptr ? blob_handler->GetSerializedData()
                                         : std::vector<uint8_t>();
    }

    GenerateCb generate_cb;
    std::shared_ptr<CustomBlobHandler> blob_handler{nullptr};
  };

  void ConvertToInternal() {
    for (auto &[id_key, id_info] : ids) {
      id_info.ConvertToInternal();
      auto [_, success] = internal_ids.try_emplace(id_key, id_info);
      env::assert_fatal(success, fmt::format("Could not save {}", id_key));
    }
  }

  std::unordered_map<IdKey, UserIdInfo> ids;
};

class CustomGenerator : public internal::BuilderInterface {
public:
  CustomGenerator(const std::string &name, const TargetEnv &env)
      : name_(name),
        env_(env.GetTargetRootDir(), env.GetTargetBuildDir() / name),
        serialization_(env_.GetTargetBuildDir() /
                       fmt::format("{}.json", name)) {
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
  void GenerateTask();

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
};

} // namespace buildcc

#endif
