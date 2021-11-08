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

#ifndef TARGET_PATH_H_
#define TARGET_PATH_H_

#include <filesystem>
#include <string>

// The Path class defined below is meant to be used with Sets
#include <unordered_set>

// Env
#include "env/assert_fatal.h"

// Third party
#include "fmt/format.h"

namespace fs = std::filesystem;

namespace buildcc::internal {

class Path {
public:
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
  static Path CreateNewPath(const fs::path &pathname) noexcept {
    return Path(pathname, 0);
  }

  // Getters
  std::uint64_t GetLastWriteTimestamp() const { return last_write_timestamp_; }
  const fs::path &GetPathname() const { return pathname_; }

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

private:
  explicit Path(const fs::path &pathname, std::uint64_t last_write_timestamp)
      : pathname_(pathname), last_write_timestamp_(last_write_timestamp) {
    pathname_.make_preferred();
  }

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

typedef std::unordered_set<Path, PathHash> path_unordered_set;
typedef std::unordered_set<fs::path, PathHash> fs_unordered_set;

// * Relation between
// - internal timestamp verified files (Path + Timestamp)
// - user facing file paths (Only Path)
// ? Why has this been done?
// We cannot guarantee that filepaths would be present
// when the user is defining the build
// The input to a Generator / Target might also be generated!
// We must only verify the File timestamp AFTER dependent Generator(s) /
// Target(s) have been built
// ? Why not do everything inside path_unordered_set?
// Users might want to query just the `fs_unordered_set` instead of the entire
// internal::path_unordered_set (The timestamp is internal information that the
// user does not need)
// In this case we opt for runtime (speed) optimization instead of memory
// optimization by caching the `user` information and `internal` information
// together
struct default_files {
  default_files() {}
  default_files(const path_unordered_set &i, const fs_unordered_set &u)
      : internal(i), user(u) {}

  void Convert() {
    if (done_once) {
      return;
    }

    done_once = true;
    for (const auto &p : user) {
      internal.emplace(Path::CreateExistingPath(p));
    }
  }

public:
  path_unordered_set internal;
  fs_unordered_set user;

private:
  bool done_once{false};
};

} // namespace buildcc::internal

namespace buildcc {

inline std::string path_as_string(const fs::path &p) {
  return internal::Path::CreateNewPath(p).GetPathAsString();
}

inline std::string path_as_string_for_display(const fs::path &p) {
  return internal::Path::CreateNewPath(p).GetPathAsStringForDisplay();
}

} // namespace buildcc

// FMT specialization

namespace fmt {

template <> struct formatter<fs::path> : formatter<std::string> {
  auto format(const fs::path &p, format_context &ctx) {
    return formatter<std::string>::format(
        buildcc::internal::Path::CreateNewPath(p).GetPathAsStringForDisplay(),
        ctx);
  }
};

template <> struct formatter<buildcc::internal::Path> : formatter<std::string> {
  auto format(const buildcc::internal::Path &p, format_context &ctx) {
    return formatter<std::string>::format(p.GetPathAsStringForDisplay(), ctx);
  }
};

} // namespace fmt

#endif
