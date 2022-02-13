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

#ifndef TARGET_SERIALIZATION_TARGET_SERIALIZATION_H_
#define TARGET_SERIALIZATION_TARGET_SERIALIZATION_H_

#include <mutex>

#include "schema/path.h"
#include "schema/serialization_interface.h"
#include "schema/target_type.h"

namespace buildcc::internal {

struct TargetSchema {
  std::string name;
  TargetType type;

  path_unordered_set internal_sources;
  path_unordered_set internal_headers;
  path_unordered_set internal_pchs;
  std::vector<Path> internal_libs;
  std::vector<std::string> external_libs;

  fs_unordered_set include_dirs;
  fs_unordered_set lib_dirs;

  std::unordered_set<std::string> preprocessor_flags;
  std::unordered_set<std::string> common_compile_flags;
  std::unordered_set<std::string> pch_compile_flags;
  std::unordered_set<std::string> pch_object_flags;
  std::unordered_set<std::string> asm_compile_flags;
  std::unordered_set<std::string> c_compile_flags;
  std::unordered_set<std::string> cpp_compile_flags;
  std::unordered_set<std::string> link_flags;

  path_unordered_set internal_compile_dependencies;
  path_unordered_set internal_link_dependencies;

  bool pch_compiled{false};
  bool target_linked{false};
};

class TargetSerialization : public SerializationInterface {
public:
  TargetSerialization(const fs::path &serialized_file)
      : SerializationInterface(serialized_file) {}

  void UpdatePchCompiled(const TargetSchema &store);
  void UpdateTargetCompiled();
  void AddSource(const internal::Path &source);
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
