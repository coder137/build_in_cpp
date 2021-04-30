#ifndef TARGET_INCLUDE_INTERNAL_PATH_H_
#define TARGET_INCLUDE_INTERNAL_PATH_H_

#include <filesystem>
#include <string>

// The Path class defined below is meant to be used with Sets
#include <unordered_set>

namespace fs = std::filesystem;

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
  static Path CreateExistingPath(const fs::path &pathname) {
    uint64_t last_write_timestamp =
        std::filesystem::last_write_time(pathname).time_since_epoch().count();
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

  // Setters
  void SetLastWriteTimestamp(std::uint64_t timestamp) {
    last_write_timestamp_ = timestamp;
  }

  // Getters
  std::uint64_t GetLastWriteTimestamp() const { return last_write_timestamp_; }
  const fs::path &GetPathname() const { return pathname_; }

  // Used during find operation
  bool operator==(const Path &p) const {
    return GetPathname() == p.GetPathname();
  }

  bool operator==(const fs::path &pathname) const {
    return GetPathname() == pathname;
  }

private:
  explicit Path(const fs::path &pathname, std::uint64_t last_write_timestamp)
      : pathname_(fs::path(pathname).make_preferred()),
        last_write_timestamp_(last_write_timestamp) {}

private:
  fs::path pathname_;
  std::uint64_t last_write_timestamp_;
};

// Used by Path
class PathHash {
public:
  size_t operator()(const Path &p) const {
    return std::hash<std::string>()(p.GetPathname().string());
  }
};

typedef std::unordered_set<Path, PathHash> path_unordered_set;

} // namespace buildcc::internal

#endif
