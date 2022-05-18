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

#ifndef TARGET_TEMPLATE_GENERATOR_H_
#define TARGET_TEMPLATE_GENERATOR_H_

#include "target/custom_generator.h"

namespace buildcc {

class TemplateGenerator : public CustomGenerator {
public:
  using CustomGenerator::CustomGenerator;
  ~TemplateGenerator() override = default;
  TemplateGenerator(const TemplateGenerator &) = delete;

  void AddTemplate(const fs::path &input_filename,
                   const fs::path &output_filename);
  std::string Parse(const std::string &pattern) const;

  /**
   * @brief Build FileGenerator Tasks
   *
   * Use `GetTaskflow` for the registered tasks
   */
  void Build() override;

  // Restrict access to certain custom generator APIs
private:
  using CustomGenerator::AddDependencyCb;
  using CustomGenerator::AddGenInfo;
  using CustomGenerator::AddGroup;
  using CustomGenerator::Build;

private:
  struct TemplateInfo {
    fs::path input;
    fs::path output;
  };

private:
  std::vector<TemplateInfo> template_infos_;
};

} // namespace buildcc

#endif
