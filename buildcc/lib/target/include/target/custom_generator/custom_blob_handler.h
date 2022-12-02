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

#ifndef TARGET_CUSTOM_GENERATOR_CUSTOM_BLOB_HANDLER_H_
#define TARGET_CUSTOM_GENERATOR_CUSTOM_BLOB_HANDLER_H_

#include <vector>

#include "env/assert_fatal.h"

namespace buildcc {

/**
 * @brief Abstract class for serializing additional data for which rebuilds
 * might be triggered i.e data that is not input/output files
 * TODO, Add examples here
 *
 */
class CustomBlobHandler {
public:
  CustomBlobHandler() = default;
  virtual ~CustomBlobHandler() = default;

  bool CheckChanged(const std::vector<uint8_t> &previous,
                    const std::vector<uint8_t> &current) const {
    env::assert_fatal(
        Verify(previous),
        "Stored blob is corrupted or User verification is incorrect");
    env::assert_fatal(
        Verify(current),
        "Current blob is corrupted or User verification is incorrect");
    return !IsEqual(previous, current);
  };

  std::vector<uint8_t> GetSerializedData() const {
    auto serialized_data = Serialize();
    env::assert_fatal(
        Verify(serialized_data),
        "Serialized data is corrupted or Serialize function is incorrect");
    return serialized_data;
  }

private:
  virtual bool Verify(const std::vector<uint8_t> &serialized_data) const = 0;
  virtual bool IsEqual(const std::vector<uint8_t> &previous,
                       const std::vector<uint8_t> &current) const = 0;
  virtual std::vector<uint8_t> Serialize() const = 0;
};

/**
 * @brief Typed Custom Blob handler which automatically performs Serialization
 * and Deserialization as long as it is JSON serializable
 *
 * NOTE: Type data is stored as a reference (to avoid copying large amount of
 * data) when constructing TypedCustomBlobHandler
 *
 * @tparam Type should be JSON serializable (see nlohmann::json compatible
 * objects)
 */
template <typename Type>
class TypedCustomBlobHandler : public CustomBlobHandler {
public:
  explicit TypedCustomBlobHandler(const Type &data) : data_(data) {}

  // serialized_data has already been verified
  static Type Deserialize(const std::vector<uint8_t> &serialized_data) {
    json j = json::from_msgpack(serialized_data, true, false);
    Type deserialized;
    j.get_to(deserialized);
    return deserialized;
  }

private:
  const Type &data_;

  bool Verify(const std::vector<uint8_t> &serialized_data) const override {
    json j = json::from_msgpack(serialized_data, true, false);
    return !j.is_discarded();
  }

  bool IsEqual(const std::vector<uint8_t> &previous,
               const std::vector<uint8_t> &current) const override {
    return Deserialize(previous) == Deserialize(current);
  }

  std::vector<uint8_t> Serialize() const override {
    json j = data_;
    return json::to_msgpack(j);
  }
};

} // namespace buildcc

#endif
