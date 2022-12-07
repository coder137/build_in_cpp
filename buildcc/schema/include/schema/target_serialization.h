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

#ifndef SCHEMA_TARGET_SERIALIZATION_H_
#define SCHEMA_TARGET_SERIALIZATION_H_

#include <mutex>

#include "schema/path.h"
#include "schema/target_schema.h"

#include "schema/interface/serialization_interface.h"

namespace buildcc::internal {

class TargetSerialization : public SerializationInterface {
public:
  TargetSerialization(const fs::path &serialized_file)
      : SerializationInterface(serialized_file) {}

  void UpdatePchCompiled(const TargetSchema &store);
  void UpdateTargetCompiled();
  void AddSource(const std::string &source, const std::string &hash);
  void UpdateStore(const TargetSchema &store);

  const TargetSchema &GetLoad() const { return load_; }
  const TargetSchema &GetStore() const { return store_; }

private:
  bool Verify(const std::string &serialized_data) override;
  bool Load(const std::string &serialized_data) override;
  bool Store(const fs::path &absolute_serialized_file) override;

private:
  TargetSchema load_;
  TargetSchema store_;

  std::mutex add_source_mutex;
};

} // namespace buildcc::internal

#endif
