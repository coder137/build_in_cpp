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

#include "schema/custom_generator_serialization.h"

// Third party
#include "nlohmann/json.hpp"

namespace buildcc::internal {

// PRIVATE

bool CustomGeneratorSerialization::Verify(const std::string &serialized_data) {
  (void)serialized_data;
  return true;
}

bool CustomGeneratorSerialization::Load(const std::string &serialized_data) {
  json j = json::parse(serialized_data, nullptr, false);
  bool loaded = !j.is_discarded();

  if (loaded) {
    try {
      load_ = j.get<CustomGeneratorSchema>();
    } catch (const std::exception &e) {
      env::log_critical(__FUNCTION__, e.what());
      loaded = false;
    }
  }
  return loaded;
}

bool CustomGeneratorSerialization::Store(
    const fs::path &absolute_serialized_file) {
  json j = store_;
  auto data = j.dump(4);
  return env::save_file(path_as_string(absolute_serialized_file).c_str(), data,
                        false);
}

} // namespace buildcc::internal
