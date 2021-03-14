#ifndef BUILDCC_INCLUDE_INTERNAL_PATH_H_
#define BUILDCC_INCLUDE_INTERNAL_PATH_H_

#include <filesystem>
#include <string>

// The Path class defined below is meant to be used with Sets
#include <unordered_set>

namespace buildcc::internal {

class Path {
public:
  /**
   * @brief Create a Existing Path object and sets last_write_timstamp to file
   * timestamp
   * NOTE, Throws filesystem exception if file not found
   *
   * @param pathname
   * @return Path
   */
  static Path CreateExistingPath(const std::string &pathname) {
    uint64_t last_write_timestamp =
        std::filesystem::last_write_time(pathname).time_since_epoch().count();
    return Path(pathname, last_write_timestamp);
  }

  static Path CreateNewPath(const std::string &pathname,
                            uint64_t last_write_timestamp) noexcept {
    return Path(pathname, last_write_timestamp);
  }

  /**
   * @brief Create a New Path object and sets last_write_timestamp to 0
   *
   * @param pathname
   * @return Path
   */
  static Path CreateNewPath(const std::string &pathname) noexcept {
    return Path(pathname, 0);
  }

  // Setters
  void SetLastWriteTimestamp(uint64_t last_write_timestamp) {
    last_write_timestamp_ = last_write_timestamp;
  }

  // Getters
  std::uint64_t GetLastWriteTimestamp() const { return last_write_timestamp_; }
  const std::string &GetPathname() const { return pathname_; }

  // Used during find operation
  bool operator==(const Path &p) const {
    return GetPathname() == p.GetPathname();
  }

  bool operator==(const std::string &pathname) const {
    return GetPathname() == pathname;
  }

private:
  explicit Path(const std::string &pathname, std::uint64_t last_write_timestamp)
      : pathname_(pathname), last_write_timestamp_(last_write_timestamp) {}

private:
  std::string pathname_;
  std::uint64_t last_write_timestamp_;
};

// Used by Path
class PathHash {
public:
  size_t operator()(const Path &p) const {
    return std::hash<std::string>()(p.GetPathname());
  }
};

typedef std::unordered_set<Path, PathHash> path_unordered_set;

} // namespace buildcc::internal

#endif
