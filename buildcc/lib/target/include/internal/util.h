/*
 * Copyright 2021 Niket Naidu All rights reserved.
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

#ifndef TARGET_INCLUDE_INTERNAL_UTIL_H_
#define TARGET_INCLUDE_INTERNAL_UTIL_H_

#include <string>
#include <vector>

#include "internal/path.h"

namespace buildcc::internal {

// System
/**
 * @brief Executes an external command
 * Internally command uses `std::system`
 * TODO, Replace with `subprocess`
 *
 * @param command_tokens
 * @return true
 * @return false
 */
bool command(const std::vector<std::string> &command_tokens);

// Additions
/**
 * @brief Existing path is stored inside stored_paths
 * Returns false if path is stored
 * Throws buildcc::env::assert_exception if path does not exist
 *
 * @param path
 * @param stored_paths
 * @return true
 * @return false
 */
bool add_path(const fs::path &path, path_unordered_set &stored_paths);

// Checks
/**
 * @brief Perform check to see if previous path is present in current path
 *
 * @param previous_paths
 * @param current_paths
 * @return true
 * @return false
 */
bool is_previous_paths_different(const path_unordered_set &previous_paths,
                                 const path_unordered_set &current_paths);

// Aggregates
std::string aggregate(const std::vector<std::string> &list);
std::string aggregate(const std::unordered_set<std::string> &list);
std::string aggregate(const buildcc::internal::path_unordered_set &paths);

std::string aggregate_with_prefix(const std::string &prefix,
                                  const std::unordered_set<std::string> &dirs);

} // namespace buildcc::internal

#endif
