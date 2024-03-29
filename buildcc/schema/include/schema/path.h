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

struct PathInfo {
private:
  static constexpr const char *const kPath = "path";
  static constexpr const char *const kHash = "hash";

public:
  PathInfo() = default;
  PathInfo(const std::string &p, const std::string &h) : path(p), hash(h) {}

  bool operator==(const PathInfo &other) const {
    return ((path == other.path) && (hash == other.hash));
  }

  /**
   * @brief Sanitizes a fs::path or std::string to a standard path string
   * - Converts backslash (\) to forward slash (/)
   * - Makes fs::lexically_normal (see std::filesystem library impl)
   *
   * @param str User provided fs::path/std::string
   * @return std::string Sanitized path as std::string
   */
  static std::string ToPathString(const fs::path &p) {
    return fs::path(p).make_preferred().lexically_normal().string();
  }

  /**
   * @brief Formats a fs::path or std::string for display
   * - All the sanitization as done in `ToPathString`
   * Additionally
   * - Adds quotation marks ("") when a space is detected
   * For example: test/hello world/ -> "test/hello world/"
   *
   * NOTE: Use this API only in places where you would like to output to
   * console/run or create command through subprocess
   *
   * @param str User provided fs::path/std::string
   * @return std::string Sanitized path as std::string for display
   */
  static std::string ToPathDisplayString(const fs::path &p) {
    auto path_str = ToPathString(p);
    // if spaces are present in the path string, surround this with brackets
    if (path_str.find(' ') != std::string::npos) {
      path_str = fmt::format("\"{}\"", path_str);
    }
    return path_str;
  }

  friend void to_json(json &j, const PathInfo &info) {
    j[kPath] = info.path;
    j[kHash] = info.hash;
  }

  friend void from_json(const json &j, PathInfo &info) {
    j.at(kPath).get_to(info.path);
    j.at(kHash).get_to(info.hash);
  }

  std::string path;
  std::string hash;
};

/**
 * @brief Stores path
 */
class PathList {
public:
  PathList() = default;
  PathList(std::initializer_list<std::string> paths) {
    for (const auto &path : paths) {
      Emplace(path);
    }
  }

  void Emplace(const fs::path &p) {
    auto path_str = PathInfo::ToPathString(p);
    paths_.emplace_back(std::move(path_str));
  }

  // TODO, Create a move version of Emplace(std::string &&pstr)

  void Insert(const PathList &other) {
    paths_.insert(paths_.end(), other.paths_.begin(), other.paths_.end());
  }

  // TODO, Create a move version of Insert (PathList &&)

  // TODO, Remove this (redundant, use operator == overload instead)
  bool IsEqual(const PathList &other) const { return paths_ == other.paths_; }

  const std::vector<std::string> &GetPaths() const { return paths_; }

  std::unordered_set<std::string> GetUnorderedPaths() const {
    std::unordered_set<std::string> unordered_paths(paths_.begin(),
                                                    paths_.end());
    return unordered_paths;
  }

  bool operator==(const PathList &other) const { return IsEqual(other); }

  friend void to_json(json &j, const PathList &plist) { j = plist.paths_; }

  friend void from_json(const json &j, PathList &plist) {
    j.get_to(plist.paths_);
  }

private:
  std::vector<std::string> paths_;
};

/**
 * @brief Stores path + path hash in a hashmap
 *
 */
class PathInfoList {
public:
  PathInfoList() = default;
  explicit PathInfoList(
      std::initializer_list<std::pair<const std::string, std::string>>
          path_infos) {
    for (const auto &pinfo : path_infos) {
      Emplace(pinfo.first, pinfo.second);
    }
  }

  void Emplace(const fs::path &p, const std::string &hash) {
    auto path_str = PathInfo::ToPathString(p);
    infos_.emplace_back(PathInfo(path_str, hash));
  }

  // TODO, Create a move version of Emplace(std::string &&pstr, std::string
  // &&hash)

  void Insert(const PathInfoList &other) {
    infos_.insert(infos_.end(), other.infos_.begin(), other.infos_.end());
  }

  // TODO, Create a move version of Insert(PathInfoList &&other)

  void ComputeHashForAll() {
    for (auto &info : infos_) {
      info.hash = ComputeHash(info.path);
    }
  }

  // TODO, Remove redundant function (use operator == overload)
  bool IsEqual(const PathInfoList &other) const {
    return infos_ == other.infos_;
  }

  const std::vector<PathInfo> &GetPathInfos() const { return infos_; }

  std::unordered_map<std::string, std::string> GetUnorderedPathInfos() const {
    std::unordered_map<std::string, std::string> unordered_path_infos;
    for (const auto &info : infos_) {
      unordered_path_infos.try_emplace(info.path, info.hash);
    }
    return unordered_path_infos;
  }

  std::vector<std::string> GetPaths() const {
    std::vector<std::string> paths;
    for (const auto &info : infos_) {
      paths.emplace_back(info.path);
    }
    return paths;
  }

  // TODO, Add Compute Strategy enum
  static std::string ComputeHash(const std::string &pstr) {
    auto path_str = PathInfo::ToPathString(pstr);

    // TODO, There might be a file checksum hash compute strategy
    // This is the timestamp hash compute strategy
    std::error_code errcode;
    const std::uint64_t last_write_timestamp =
        std::filesystem::last_write_time(path_str, errcode)
            .time_since_epoch()
            .count();
    env::assert_fatal(errcode.value() == 0,
                      fmt::format("{} not found", path_str));
    return std::to_string(last_write_timestamp);
  }

  bool operator==(const PathInfoList &other) const { return IsEqual(other); }

  friend void to_json(json &j, const PathInfoList &plist) { j = plist.infos_; }

  friend void from_json(const json &j, PathInfoList &plist) {
    j.get_to(plist.infos_);
  }

private:
  std::vector<PathInfo> infos_;
};

} // namespace buildcc::internal

namespace buildcc {

inline std::string path_as_string(const fs::path &p) {
  return internal::PathInfo::ToPathString(p);
}

inline std::string path_as_display_string(const fs::path &p) {
  return internal::PathInfo::ToPathDisplayString(p);
}

inline fs::path string_as_path(const std::string &p) {
  return path_as_string(p);
}

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
