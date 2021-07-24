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

#ifndef ENV_ENV_H_
#define ENV_ENV_H_

#include <string>

#include <filesystem>

namespace fs = std::filesystem;

namespace buildcc::env {

/**
 * @brief Initialize project environment
 *
 * @param project_root_dir Root directory for source files
 * @param project_build_dir Directory for intermediate build files
 */
void init(const fs::path &project_root_dir, const fs::path &project_build_dir);
void deinit();

// Getters
bool is_init();
const fs::path &get_project_root_dir();
const fs::path &get_project_build_dir();

} // namespace buildcc::env

#endif
