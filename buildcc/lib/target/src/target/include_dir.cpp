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
void Target::AddIncludeDir(const std::string &relative_include_dir) {
  env::log_trace(__FUNCTION__, name_);

  const std::string absolute_include_dir =
      (target_root_source_dir_ / relative_include_dir)
          .make_preferred()
          .string();
  if (current_include_dirs_.find(absolute_include_dir) !=
      current_include_dirs_.end()) {
    return;
  }

  current_include_dirs_.insert(absolute_include_dir);
  std::string temp{""};
  aggregated_include_dirs_ += temp.append(prefix_include_dir_)
                                  .append(internal::quote(absolute_include_dir))
                                  .append(" ");
}

} // namespace buildcc::base
