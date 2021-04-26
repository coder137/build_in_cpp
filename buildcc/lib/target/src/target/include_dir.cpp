#include "target.h"

#include "assert_fatal.h"
#include "logging.h"

#include "internal/util.h"

namespace buildcc::base {

void Target::AddHeader(const std::string &relative_filename,
                       const fs::path &relative_to_target_path) {
  env::log_trace(__FUNCTION__, name_);

  // Check Source
  fs::path absolute_filepath =
      target_root_source_dir_ / relative_to_target_path / relative_filename;

  internal::add_path(absolute_filepath, current_header_files_);
}

void Target::AddHeader(const std::string &relative_filename) {
  AddHeader(relative_filename, "");
}

// Public
// TODO, Change the functionality of this
void Target::AddIncludeDir(const std::string &relative_include_dir) {
  env::log_trace(__FUNCTION__, name_);

  fs::path absolute_include_dir =
      target_root_source_dir_ / relative_include_dir;

  auto current_dir =
      buildcc::internal::Path::CreateNewPath(absolute_include_dir);
  if (current_include_dirs_.find(current_dir) != current_include_dirs_.end()) {
    return;
  }

  uint64_t max_timestamp_count = 0;
  for (const auto d : fs::directory_iterator(absolute_include_dir)) {
    max_timestamp_count = std::max(
        max_timestamp_count,
        static_cast<uint64_t>(d.last_write_time().time_since_epoch().count()));
  }
  current_dir.SetLastWriteTimestamp(max_timestamp_count);
  current_include_dirs_.insert(current_dir);
}

} // namespace buildcc::base
