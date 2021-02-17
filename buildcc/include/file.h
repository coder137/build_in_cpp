#ifndef BUILDCC_INCLUDE_FILE_H_
#define BUILDCC_INCLUDE_FILE_H_

#include <filesystem>
#include <string>

namespace buildcc::internal {

class File {
public:
  File(const std::string &filename)
      : filename_(filename),
        last_write_timestamp_(std::filesystem::last_write_time(filename)
                                  .time_since_epoch()
                                  .count()) {}

  File(const std::string &filename, std::uint64_t last_write_timestamp)
      : filename_(filename), last_write_timestamp_(last_write_timestamp) {}

  // Setters
  void SetLastWriteTimestamp(uint64_t last_write_timestamp) {
    last_write_timestamp_ = last_write_timestamp;
  }

  // Getters
  std::uint64_t GetLastWriteTimestamp() const { return last_write_timestamp_; }
  const std::string &GetFilename() const { return filename_; }

  // Used during find operation
  bool operator==(const File &f) const {
    return GetFilename() == f.GetFilename();
  }

private:
  std::string filename_;
  std::uint64_t last_write_timestamp_;
};

// Used by File
class FileHash {
public:
  size_t operator()(const File &f) const {
    return std::hash<std::string>()(f.GetFilename());
  }
};

} // namespace buildcc::internal

#endif
