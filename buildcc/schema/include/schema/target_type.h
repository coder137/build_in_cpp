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

#ifndef SCHEMA_TARGET_TYPE_H_
#define SCHEMA_TARGET_TYPE_H_

#include <array>
#include <optional>
#include <string_view>

namespace buildcc {

enum class TargetType {
  Executable,     ///< Executable Target type
  StaticLibrary,  ///< Static library target type
  DynamicLibrary, ///< Dynamic library target type
  Undefined,      ///< Undefined target type
};

constexpr std::array<std::pair<std::string_view, TargetType>, 4>
    kTargetTypePair{
        std::make_pair("executable", TargetType::Executable),
        std::make_pair("static_library", TargetType::StaticLibrary),
        std::make_pair("dynamic_library", TargetType::DynamicLibrary),
    };

template <typename JsonType> void to_json(JsonType &j, TargetType type) {
  std::optional<std::string> name;
  for (const auto &[p_name, p_type] : kTargetTypePair) {
    // type provided should match the constexpr present pair_type
    if (type == p_type) {
      name = p_name;
      break;
    }
  }

  if (name.has_value()) {
    j = name.value();
  } else {
    j = nullptr;
  }
}

template <typename JsonType>
void from_json(const JsonType &j, TargetType &type) {
  type = TargetType::Undefined;
  if (j.is_string()) {
    std::string name;
    j.get_to(name);

    for (const auto &[p_name, p_type] : kTargetTypePair) {
      // name provided in json schema should matched the constexpr present
      // pair_name
      if (name == p_name) {
        type = p_type;
        break;
      }
    }
  }
}

} // namespace buildcc

#endif
