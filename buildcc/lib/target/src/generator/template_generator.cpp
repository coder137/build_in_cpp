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

namespace buildcc {

void TemplateGenerator::AddTemplate(const fs::path &input_filename,
                                    const fs::path &output_filename) {
  (void)input_filename;
  (void)output_filename;
}
void TemplateGenerator::AddTemplate(const std::string &pattern,
                                    fs::path &output_filename) {
  (void)pattern;
  (void)output_filename;
}

/**
 * @brief Build FileGenerator Tasks
 *
 * Use `GetTaskflow` for the registered tasks
 */
void TemplateGenerator::Build() {}

} // namespace buildcc
