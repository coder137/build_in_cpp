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
  static constexpr const char *const kSchemaName = "name";
  static constexpr const char *const kIdsName = "ids";
  static constexpr const char *const kGroupsName = "groups";

public:
  using IdKey = std::string;
  using GroupKey = std::string;
  struct IdInfo {
  private:
    static constexpr const char *const kInputsName = "inputs";
    static constexpr const char *const kOutputsName = "outputs";
    static constexpr const char *const kUserblobName = "userblob";

  public:
    path_unordered_set internal_inputs;
    fs_unordered_set outputs;
    std::vector<uint8_t> userblob;

    friend void to_json(json &j, const IdInfo &info) {
      j[kInputsName] = info.internal_inputs;
      j[kOutputsName] = info.outputs;
      j[kUserblobName] = info.userblob;
    }

    friend void from_json(const json &j, IdInfo &info) {
      j.at(kInputsName).get_to(info.internal_inputs);
      j.at(kOutputsName).get_to(info.outputs);
      j.at(kUserblobName).get_to(info.userblob);
    }
  };

  using IdPair = std::pair<const IdKey, IdInfo>;
  using GroupInfo = std::unordered_set<std::string>;
  using GroupPair = std::pair<const GroupKey, GroupInfo>;

  std::string name;
  std::unordered_map<IdKey, IdInfo> internal_ids;
  std::unordered_map<GroupKey, GroupInfo> internal_groups;

  friend void to_json(json &j, const CustomGeneratorSchema &schema) {
    j[kSchemaName] = schema.name;
    j[kIdsName] = schema.internal_ids;
    j[kGroupsName] = schema.internal_groups;
  }

  friend void from_json(const json &j, CustomGeneratorSchema &schema) {
    j.at(kSchemaName).get_to(schema.name);
    j.at(kIdsName).get_to(schema.internal_ids);
    j.at(kGroupsName).get_to(schema.internal_groups);
  }
};

} // namespace buildcc::internal

#endif
