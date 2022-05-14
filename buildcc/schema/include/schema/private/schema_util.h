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

#ifndef SCHEMA_PRIVATE_SCHEMA_UTIL_H_
#define SCHEMA_PRIVATE_SCHEMA_UTIL_H_

#include "path_generated.h"

#include <algorithm>

#include "schema/path.h"

namespace fbs = schema::internal;

namespace buildcc::internal {

// extract APIs for LOAD

inline void extract_path(
    const flatbuffers::Vector<flatbuffers::Offset<schema::internal::Path>>
        *fbs_paths,
    path_unordered_set &out) {
  if (fbs_paths == nullptr) {
    return;
  }

  for (auto iter = fbs_paths->begin(); iter != fbs_paths->end(); iter++) {
    out.insert(Path::CreateNewPath(iter->pathname()->c_str(),
                                   iter->last_write_timestamp()));
  }
}

inline void extract_path(
    const flatbuffers::Vector<flatbuffers::Offset<schema::internal::Path>>
        *fbs_paths,
    std::vector<Path> &out) {
  if (fbs_paths == nullptr) {
    return;
  }

  for (auto iter = fbs_paths->begin(); iter != fbs_paths->end(); iter++) {
    out.push_back(Path::CreateNewPath(iter->pathname()->c_str(),
                                      iter->last_write_timestamp()));
  }
}

inline void extract(const flatbuffers::Vector<uint8_t> *fbs_data,
                    std::vector<uint8_t> &out) {
  if (fbs_data == nullptr) {
    return;
  }

  for (auto iter = fbs_data->begin(); iter != fbs_data->end(); iter++) {
    out.push_back(*iter);
  }
}

inline void
extract(const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>
            *fbs_paths,
        fs_unordered_set &out) {
  if (fbs_paths == nullptr) {
    return;
  }

  for (auto iter = fbs_paths->begin(); iter != fbs_paths->end(); iter++) {
    out.insert(iter->str());
  }
}

template <typename T>
inline void
extract(const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>
            *fbs_paths,
        std::unordered_set<T> &out) {
  if (fbs_paths == nullptr) {
    return;
  }

  for (auto iter = fbs_paths->begin(); iter != fbs_paths->end(); iter++) {
    out.insert(iter->str());
  }
}

template <typename T>
inline void
extract(const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>
            *fbs_paths,
        std::vector<T> &out) {
  if (fbs_paths == nullptr) {
    return;
  }

  for (auto iter = fbs_paths->begin(); iter != fbs_paths->end(); iter++) {
    out.push_back(iter->str());
  }
}

// Create APIs for STORE

template <typename T>
inline std::vector<flatbuffers::Offset<fbs::Path>>
create_fbs_vector_path(flatbuffers::FlatBufferBuilder &builder,
                       const T &pathlist) {
  std::vector<flatbuffers::Offset<fbs::Path>> paths;
  for (const auto &p : pathlist) {
    auto fbs_file = fbs::CreatePathDirect(builder, p.GetPathAsString().c_str(),
                                          p.GetLastWriteTimestamp());
    paths.push_back(fbs_file);
  }
  return paths;
}

template <typename T>
inline std::vector<flatbuffers::Offset<flatbuffers::String>>
create_fbs_vector_string(flatbuffers::FlatBufferBuilder &builder,
                         const T &strlist) {
  std::vector<flatbuffers::Offset<flatbuffers::String>> strs;
  std::transform(
      strlist.begin(), strlist.end(), std::back_inserter(strs),
      [&](const std::string &str) -> flatbuffers::Offset<flatbuffers::String> {
        return builder.CreateString(str);
      });
  return strs;
}

inline std::vector<flatbuffers::Offset<flatbuffers::String>>
create_fbs_vector_string(flatbuffers::FlatBufferBuilder &builder,
                         const buildcc::fs_unordered_set &fslist) {
  std::vector<flatbuffers::Offset<flatbuffers::String>> strs;
  std::transform(
      fslist.begin(), fslist.end(), std::back_inserter(strs),
      [&](const fs::path &p) -> flatbuffers::Offset<flatbuffers::String> {
        return builder.CreateString(path_as_string(p));
      });
  return strs;
}

} // namespace buildcc::internal

#endif
