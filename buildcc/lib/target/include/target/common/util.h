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

#ifndef TARGET_COMMON_UTIL_H_
#define TARGET_COMMON_UTIL_H_

#include <string>
#include <vector>

#include "schema/path.h"

namespace buildcc::internal {

// Aggregates
template <typename T> std::string aggregate(const T &list) {
  return fmt::format("{}", fmt::join(list, " "));
}

template <typename T>
std::string aggregate_with_prefix(const std::string &prefix, const T &list) {
  std::vector<std::string> agg_list;
  for (const auto &l : list) {
    auto formatted_output = fmt::format("{}{}", prefix, l);
    agg_list.emplace_back(std::move(formatted_output));
  }
  return aggregate(agg_list);
}

} // namespace buildcc::internal

#endif
