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

namespace buildcc {

enum class TargetType {
  Executable,     ///< Executable Target type
  StaticLibrary,  ///< Static library target type
  DynamicLibrary, ///< Dynamic library target type
  Undefined,      ///< Undefined target type
};

template <typename JsonType> void to_json(JsonType &j, TargetType type) {
  switch (type) {
  case TargetType::Executable:
    j = "executable";
    break;
  case TargetType::StaticLibrary:
    j = "static_library";
    break;
  case TargetType::DynamicLibrary:
    j = "dynamic_library";
    break;
  default:
    j = nullptr;
    break;
  }
}

template <typename JsonType>
void from_json(const JsonType &j, TargetType &type) {
  if (j.is_null()) {
    type = TargetType::Undefined;
  } else {
    j.get_to(type);
  }
}

} // namespace buildcc

#endif
