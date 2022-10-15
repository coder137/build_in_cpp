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

#include "target/template_generator.h"

#include "env/env.h"

namespace {

bool template_generate_cb(const buildcc::CustomGeneratorContext &ctx) {
  std::string pattern_data;
  const fs::path &input = *ctx.inputs.begin();
  const fs::path &output = *ctx.outputs.begin();

  bool success =
      buildcc::env::load_file(input.string().c_str(), false, &pattern_data);
  if (success) {
    std::string parsed_data = ctx.command.Construct(pattern_data);
    success =
        buildcc::env::save_file(output.string().c_str(), parsed_data, false);
  }

  if (!success) {
    buildcc::env::log_critical(
        __FUNCTION__, fmt::format("Failed to parse {} -> {}", input, output));
  }
  return success;
}

} // namespace

namespace buildcc {

void TemplateGenerator::AddTemplate(std::string_view absolute_input_pattern,
                                    std::string_view absolute_output_pattern) {
  TemplateInfo info;
  info.input_pattern = absolute_input_pattern;
  info.output_pattern = absolute_output_pattern;
  template_infos_.emplace_back(std::move(info));
}

std::string TemplateGenerator::Parse(const std::string &pattern) const {
  return ParsePattern(pattern);
}

/**
 * @brief Build FileGenerator Tasks
 *
 * Use `GetTaskflow` for the registered tasks
 */
void TemplateGenerator::Build() {
  for (const auto &info : template_infos_) {
    std::string name = string_as_path(ParsePattern(info.input_pattern))
                           .lexically_relative(Project::GetRootDir())
                           .string();
    AddGenInfo(name, {info.input_pattern}, {info.output_pattern},
               template_generate_cb);
  }
  this->CustomGenerator::Build();
}

} // namespace buildcc
