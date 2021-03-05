#ifndef BUILDCC_INCLUDE_PATH_H_
#define BUILDCC_INCLUDE_PATH_H_

#include <filesystem>
#include <string>

namespace buildcc::internal {

class Path {
public:
  static Path CreateExistingPath(const std::string &pathname) {
    return Path(pathname);
  }

  static Path CreateNewPath(const std::string &pathname,
                            uint64_t last_write_timestamp) {
    return Path(pathname, last_write_timestamp);
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
  explicit Path(const std::string &pathname)
      : pathname_(pathname),
        last_write_timestamp_(std::filesystem::last_write_time(pathname)
                                  .time_since_epoch()
                                  .count()) {}

  explicit Path(const std::string &pathname,
                std::uint64_t last_write_timestamp) noexcept
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

} // namespace buildcc::internal

#endif
