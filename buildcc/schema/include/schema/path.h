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

#ifndef SCHEMA_PATH_H_
#define SCHEMA_PATH_H_

#include <filesystem>
#include <functional>
#include <string>
#include <unordered_set>

// Env
#include "env/assert_fatal.h"

// Third party
#include "fmt/format.h"
#include "nlohmann/json.hpp"

namespace fs = std::filesystem;
using json = nlohmann::ordered_json;

namespace buildcc::internal {

class Path {
private:
  static constexpr const char *const kPathName = "path";
  static constexpr const char *const kHashName = "hash";

public:
  Path() = default;
  explicit Path(const fs::path &pathname, std::uint64_t last_write_timestamp)
      : pathname_(pathname), last_write_timestamp_(last_write_timestamp) {
    pathname_.lexically_normal().make_preferred();
  }

  /**
   * @brief Create a Existing Path object and sets last_write_timstamp to file
   * timestamp
   * NOTE, Throws buildcc::env::assert_exception if file not found
   *
   * @param pathname
   * @return Path
   */
  static Path CreateExistingPath(const fs::path &pathname) {
    std::error_code errcode;
    uint64_t last_write_timestamp =
        std::filesystem::last_write_time(pathname, errcode)
            .time_since_epoch()
            .count();
    env::assert_fatal(errcode.value() == 0,
                      fmt::format("{} not found", pathname));

    return Path(pathname, last_write_timestamp);
  }

  static Path CreateNewPath(const fs::path &pathname,
                            uint64_t last_write_timestamp) noexcept {
    return Path(pathname, last_write_timestamp);
  }

  /**
   * @brief Create a New Path object and sets last_write_timestamp to 0
   *
   * @param pathname
   * @return Path
   */
  static Path CreateNewPath(const fs::path &pathname) {
    return Path(pathname, 0);
  }

  // Getters
  std::uint64_t GetLastWriteTimestamp() const noexcept {
    return last_write_timestamp_;
  }
  const fs::path &GetPathname() const noexcept { return pathname_; }

  /**
   * @brief Get fs::path as std::string while keeping the preferred os
   * path delimiters
   * '\\' for windows and '/' for linux
   *
   * @return std::string
   */
  std::string GetPathAsString() const { return GetPathname().string(); }

  /**
   * @brief Get fs::path as std::string for display
   * Converts '\\' to '/' for conformity
   *
   * @return std::string
   */
  std::string GetPathAsStringForDisplay() const {
    return Quote(ConvertPathToString());
  }

  // Used during find operation
  bool operator==(const Path &p) const {
    return GetPathname() == p.GetPathname();
  }

  bool operator==(const fs::path &pathname) const {
    return GetPathname() == pathname;
  }

  // JSON specialization

  friend void to_json(json &j, const Path &p) {
    j = json{
        {kPathName, p.pathname_},
        {kHashName, p.last_write_timestamp_},
    };
  }

  friend void from_json(const json &j, Path &p) {
    j.at(kPathName).get_to(p.pathname_);
    j.at(kHashName).get_to(p.last_write_timestamp_);
  }

private:
  std::string Quote(const std::string &str) const {
    if (str.find(" ") == std::string::npos) {
      return str;
    }
    return fmt::format("\"{}\"", str);
  }

  std::string ConvertPathToString() const {
    std::string pstr = pathname_.string();
    std::replace(pstr.begin(), pstr.end(), '\\', '/');
    return pstr;
  }

private:
  fs::path pathname_;
  std::uint64_t last_write_timestamp_;
};

// Used by Path
class PathHash {
public:
  size_t operator()(const Path &p) const {
    return fs::hash_value(p.GetPathname());
  }

  size_t operator()(const fs::path &p) const { return fs::hash_value(p); }
};

using path_unordered_set = std::unordered_set<Path, PathHash>;
using fs_unordered_set = std::unordered_set<fs::path, PathHash>;

inline std::vector<Path>
path_schema_convert(const std::vector<fs::path> &path_list,
                    const std::function<Path(const fs::path &)> &cb =
                        Path::CreateExistingPath) {
  std::vector<Path> internal_path_list;
  for (const auto &p : path_list) {
    internal_path_list.push_back(cb(p));
  }
  return internal_path_list;
}

inline path_unordered_set
path_schema_convert(const fs_unordered_set &path_set,
                    const std::function<Path(const fs::path &)> &cb =
                        Path::CreateExistingPath) {
  path_unordered_set internal_path_set;
  for (const auto &p : path_set) {
    internal_path_set.insert(cb(p));
  }
  return internal_path_set;
}

inline fs_unordered_set
path_schema_convert(const path_unordered_set &internal_path_set) {
  fs_unordered_set path_set;
  for (const auto &p : internal_path_set) {
    path_set.insert(p.GetPathname());
  }
  return path_set;
}

} // namespace buildcc::internal

namespace buildcc {

inline std::string path_as_string(const fs::path &p) {
  return internal::Path::CreateNewPath(p).GetPathAsString();
}

inline std::string path_as_display_string(const fs::path &p) {
  return internal::Path::CreateNewPath(p).GetPathAsStringForDisplay();
}

inline fs::path string_as_path(const std::string &str) {
  return internal::Path::CreateNewPath(str).GetPathname();
}

using fs_unordered_set = internal::fs_unordered_set;

} // namespace buildcc

// FMT specialization

template <> struct fmt::formatter<fs::path> : formatter<std::string> {
  template <typename FormatContext>
  auto format(const fs::path &p, FormatContext &ctx) {
    return formatter<std::string>::format(buildcc::path_as_display_string(p),
                                          ctx);
  }
};

#endif
