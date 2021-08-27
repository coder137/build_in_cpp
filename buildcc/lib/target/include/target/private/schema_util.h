/*
 * Copyright 2021 Niket Naidu. All rights reserved.
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

#ifndef TARGET_PRIVATE_SCHEMA_UTIL_H_
#define TARGET_PRIVATE_SCHEMA_UTIL_H_

#include "path_generated.h"

#include "target/path.h"

namespace fbs = schema::internal;

namespace buildcc::internal {

inline void ExtractPath(
    const flatbuffers::Vector<flatbuffers::Offset<schema::internal::Path>>
        *fbs_paths,
    buildcc::internal::path_unordered_set &out) {
  if (fbs_paths == nullptr) {
    return;
  }

  for (auto iter = fbs_paths->begin(); iter != fbs_paths->end(); iter++) {
    out.insert(buildcc::internal::Path::CreateNewPath(
        iter->pathname()->c_str(), iter->last_write_timestamp()));
  }
}

inline void
Extract(const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>
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
Extract(const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>
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
Extract(const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>
            *fbs_paths,
        std::vector<T> &out) {
  if (fbs_paths == nullptr) {
    return;
  }

  for (auto iter = fbs_paths->begin(); iter != fbs_paths->end(); iter++) {
    out.push_back(iter->str());
  }
}

} // namespace buildcc::internal

#endif
