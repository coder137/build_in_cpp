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

#ifndef SCHEMA_CUSTOM_GENERATOR_SERIALIZATION_H_
#define SCHEMA_CUSTOM_GENERATOR_SERIALIZATION_H_

#include <string>
#include <unordered_set>
#include <vector>

#include "schema/path.h"

#include "schema/interface/serialization_interface.h"

namespace buildcc::internal {

struct GenInfo {
  path_unordered_set internal_inputs;
  fs_unordered_set outputs;
};

struct CustomGeneratorSchema {
  std::string name;
  std::unordered_map<std::string, GenInfo> internal_gen_info_map;
};

class CustomGeneratorSerialization : public SerializationInterface {
public:
  CustomGeneratorSerialization(const fs::path &serialized_file)
      : SerializationInterface(serialized_file) {}

  void UpdateStore(const CustomGeneratorSchema &store) { store_ = store; }
  const CustomGeneratorSchema &GetLoad() const { return load_; }
  const CustomGeneratorSchema &GetStore() const { return store_; }

private:
  bool Verify(const std::string &serialized_data) override;
  bool Load(const std::string &serialized_data) override;
  bool Store(const fs::path &absolute_serialized_file) override;

private:
  CustomGeneratorSchema load_;
  CustomGeneratorSchema store_;
};

} // namespace buildcc::internal

#endif
