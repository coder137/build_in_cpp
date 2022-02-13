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

#ifndef SCHEMA_GENERATOR_SERIALIZATION_H_
#define SCHEMA_GENERATOR_SERIALIZATION_H_

#include <string>
#include <unordered_set>
#include <vector>

#include "schema/path.h"

#include "schema/interface/serialization_interface.h"

namespace buildcc::internal {

struct GeneratorSchema {
  std::string name;
  path_unordered_set internal_inputs;
  fs_unordered_set outputs;
  std::vector<std::string> commands;
};

class GeneratorSerialization : public SerializationInterface {
public:
  GeneratorSerialization(const fs::path &serialized_file)
      : SerializationInterface(serialized_file) {}

  void UpdateStore(const GeneratorSchema &store) { store_ = store; }
  const GeneratorSchema &GetLoad() const { return load_; }
  const GeneratorSchema &GetStore() const { return store_; }

private:
  bool Verify(const std::string &serialized_data) override;
  bool Load(const std::string &serialized_data) override;
  bool Store(const fs::path &absolute_serialized_file) override;

private:
  GeneratorSchema load_;
  GeneratorSchema store_;
};

} // namespace buildcc::internal

#endif
