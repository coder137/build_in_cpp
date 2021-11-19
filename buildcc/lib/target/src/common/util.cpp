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

#include "target/common/util.h"

#include "env/assert_fatal.h"
#include "env/env.h"

#include "fmt/format.h"

namespace buildcc::internal {

// Aggregates

std::string aggregate(const buildcc::internal::fs_unordered_set &paths) {
  std::vector<std::string> agg;
  std::transform(
      paths.begin(), paths.end(), std::back_inserter(agg),
      [](const fs::path &p) -> std::string { return fmt::format("{}", p); });
  return aggregate(agg);
}

std::string aggregate_with_prefix(const std::string &prefix,
                                  const fs_unordered_set &dirs) {
  std::vector<std::string> agg;
  std::transform(dirs.begin(), dirs.end(), std::back_inserter(agg),
                 [&](const fs::path &dir) -> std::string {
                   return fmt::format("{}{}", prefix, fmt::format("{}", dir));
                 });
  return aggregate(agg);
}

} // namespace buildcc::internal
