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

#include "schema/target_serialization.h"

namespace buildcc::internal {

// PUBLIC
void TargetSerialization::UpdatePchCompiled(const TargetSchema &store) {
  store_.pchs = store.pchs;
  store_.pch_compiled = true;
}

// TODO, When you add source here you need to add it with the hash
void TargetSerialization::AddSource(const std::string &source,
                                    const std::string &hash) {
  std::scoped_lock guard(add_source_mutex);
  store_.sources.Emplace(source, hash);
}

void TargetSerialization::UpdateTargetCompiled() {
  store_.target_linked = true;
}

void TargetSerialization::UpdateStore(const TargetSchema &store) {
  TargetSchema temp = store;
  temp.pchs = store_.pchs;
  temp.pch_compiled = store_.pch_compiled;
  temp.sources = store_.sources;
  temp.target_linked = store_.target_linked;
  store_ = std::move(temp);
}

// PRIVATE
bool TargetSerialization::Verify(const std::string &serialized_data) {
  (void)serialized_data;
  return true;
}

bool TargetSerialization::Load(const std::string &serialized_data) {
  json j = json::parse(serialized_data, nullptr, false);
  bool loaded = !j.is_discarded();

  if (loaded) {
    try {
      load_ = j.get<TargetSchema>();
    } catch (const std::exception &e) {
      env::log_critical(__FUNCTION__, e.what());
      loaded = false;
    }
  }
  return loaded;
}

bool TargetSerialization::Store(const fs::path &absolute_serialized_file) {
  json j = store_;
  auto data = j.dump(4);
  return env::save_file(path_as_string(absolute_serialized_file).c_str(), data,
                        false);
}

} // namespace buildcc::internal
