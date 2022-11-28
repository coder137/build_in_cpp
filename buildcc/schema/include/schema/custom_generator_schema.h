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

#ifndef SCHEMA_CUSTOM_GENERATOR_SCHEMA_H_
#define SCHEMA_CUSTOM_GENERATOR_SCHEMA_H_

#include <string>

#include "schema/path.h"

namespace buildcc::internal {

struct CustomGeneratorSchema {
private:
  static constexpr const char *const kName = "name";
  static constexpr const char *const kIds = "ids";

public:
  using IdKey = std::string;
  struct IdInfo {
  private:
    static constexpr const char *const kInputs = "inputs";
    static constexpr const char *const kOutputs = "outputs";
    static constexpr const char *const kUserblob = "userblob";

  public:
    PathInfoList inputs;
    PathList outputs;
    std::vector<uint8_t> userblob;

    friend void to_json(json &j, const IdInfo &info) {
      j[kInputs] = info.inputs;
      j[kOutputs] = info.outputs;
      j[kUserblob] = info.userblob;
    }

    friend void from_json(const json &j, IdInfo &info) {
      j.at(kInputs).get_to(info.inputs);
      j.at(kOutputs).get_to(info.outputs);
      j.at(kUserblob).get_to(info.userblob);
    }
  };

  using IdPair = std::pair<const IdKey, IdInfo>;

  std::string name;
  std::unordered_map<IdKey, IdInfo> internal_ids;

  friend void to_json(json &j, const CustomGeneratorSchema &schema) {
    j[kName] = schema.name;
    j[kIds] = schema.internal_ids;
  }

  friend void from_json(const json &j, CustomGeneratorSchema &schema) {
    j.at(kName).get_to(schema.name);
    j.at(kIds).get_to(schema.internal_ids);
  }
};

} // namespace buildcc::internal

#endif
