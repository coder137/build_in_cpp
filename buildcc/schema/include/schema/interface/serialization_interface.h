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

#ifndef SCHEMA_INTERFACE_SERIALIZATION_INTERFACE_H_
#define SCHEMA_INTERFACE_SERIALIZATION_INTERFACE_H_

#include <filesystem>

#include "env/assert_fatal.h"
#include "env/util.h"

#include "schema/path.h"

namespace fs = std::filesystem;

namespace buildcc::internal {

class SerializationInterface {
public:
  SerializationInterface(const fs::path &serialized_file)
      : serialized_file_(serialized_file) {}
  virtual ~SerializationInterface() {}

  bool LoadFromFile() {
    std::string buffer;

    // Read from serialized file
    bool is_loaded =
        env::load_file(path_as_string(serialized_file_).c_str(), true, &buffer);
    if (!is_loaded) {
      return false;
    }

    // Verify serialized data as per schema
    if (!Verify(buffer)) {
      return false;
    }

    // Load serialized data as C++ data
    loaded_ = Load(buffer);
    return loaded_;
  }

  bool StoreToFile() { return Store(serialized_file_); }

  const fs::path &GetSerializedFile() const noexcept {
    return serialized_file_;
  }
  bool IsLoaded() const noexcept { return loaded_; }

private:
  virtual bool Verify(const std::string &serialized_data) = 0;
  virtual bool Load(const std::string &serialized_data) = 0;
  virtual bool Store(const fs::path &absolute_serialized_file) = 0;

private:
  fs::path serialized_file_;
  bool loaded_{false};
};

} // namespace buildcc::internal

#endif
