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

#include "target/util.h"

#include "env/assert_fatal.h"
#include "env/env.h"

#include "fmt/format.h"

namespace buildcc::internal {

// rechecks
bool is_previous_paths_different(const path_unordered_set &previous_paths,
                                 const path_unordered_set &current_paths) {
  return std::any_of(previous_paths.begin(), previous_paths.end(),
                     [&](const internal::Path &p) {
                       return current_paths.find(p) == current_paths.end();
                     });
}

// Additions
bool add_path(const fs::path &path, path_unordered_set &stored_paths) {
  auto current_file = buildcc::internal::Path::CreateExistingPath(path);

  // TODO, Note, we might not require this check
  env::assert_fatal(stored_paths.find(current_file) == stored_paths.end(),
                    fmt::format("{} duplicate found", path.string()));

  auto [_, added] = stored_paths.insert(current_file);
  return added;
}

// Aggregates

std::string aggregate(const std::vector<std::string> &list) {
  return fmt::format("{}", fmt::join(list, " "));
}

std::string aggregate(const std::unordered_set<std::string> &list) {
  return fmt::format("{}", fmt::join(list, " "));
}

std::string aggregate(const buildcc::internal::path_unordered_set &paths) {
  std::vector<std::string> agg;
  std::transform(paths.begin(), paths.end(), std::back_inserter(agg),
                 [](const buildcc::internal::Path &p) -> std::string {
                   return p.GetPathAsString();
                 });
  return aggregate(agg);
}

std::string aggregate_with_prefix(const std::string &prefix,
                                  const fs_unordered_set &dirs) {
  std::vector<std::string> agg;
  std::transform(dirs.begin(), dirs.end(), std::back_inserter(agg),
                 [&](const fs::path &dir) -> std::string {
                   return fmt::format("{}{}", prefix, quote(dir.string()));
                 });
  return aggregate(agg);
}

} // namespace buildcc::internal
