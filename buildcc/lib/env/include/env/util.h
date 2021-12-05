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

/*
 * Copyright 2014 Google Inc. All rights reserved.
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

#ifndef ENV_UTIL_H_
#define ENV_UTIL_H_

#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>

namespace fs = std::filesystem;

namespace buildcc::env {

/**
 * Condition under which code throws and should terminate
 * 1: ofs.write -> badbit
 */
inline bool save_file(const char *name, const char *buf, size_t len,
                      bool binary) {
  if (buf == nullptr) {
    return false;
  }
  std::ofstream ofs(name, binary ? std::ofstream::binary : std::ofstream::out);
  if (!ofs.is_open()) {
    return false;
  }

  // * 1
  std::ostream &os = ofs.write(buf, len);
  return !os.bad();
}

inline bool save_file(const char *name, const std::string &buf, bool binary) {
  return save_file(name, buf.c_str(), buf.size(), binary);
}

/**
 * Condition under which code throws and should terminate
 * 1: fs::file_size -> filesystem_error, bad_alloc error
 * 2: resize -> length_error, bad_alloc error
 * 3: ifs.read -> badbit
 * 4:
 */
inline bool load_file(const char *name, bool binary, std::string *buf) {
  if (name == nullptr || buf == nullptr) {
    return false;
  }
  std::error_code errcode;
  if (fs::is_directory(name, errcode)) {
    return false;
  }
  if (errcode) {
    return false;
  }
  std::ifstream ifs(name, binary ? std::ifstream::binary : std::ifstream::in);
  if (!ifs.is_open()) {
    return false;
  }
  if (binary) {
    // The fastest way to read a file into a string.
    // If we cannot get the file size we should terminate
    // * 1
    auto size = static_cast<size_t>(fs::file_size(name));
    // * 2
    buf->resize(size);
    // * 3
    // flawfinder: ignore
    ifs.read(buf->data(), buf->size());
  } else {
    // This is slower, but works correctly on all platforms for text files.
    // * 4
    std::ostringstream oss;
    oss << ifs.rdbuf();
    *buf = oss.str();
  }
  return !ifs.bad();
}

// TODO, Shift this to a common apis folder or something
// https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
inline std::vector<std::string> split(const std::string &s, char delim) {
  std::vector<std::string> result;
  std::stringstream ss(s);
  std::string item;
  while (getline(ss, item, delim)) {
    result.push_back(item);
  }

  return result;
}

} // namespace buildcc::env

#endif
