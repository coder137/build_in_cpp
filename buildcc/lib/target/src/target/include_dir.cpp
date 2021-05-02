#include "target.h"

#include "assert_fatal.h"
#include "logging.h"

#include "internal/util.h"

#include "fmt/format.h"

namespace buildcc::base {

void Target::AddHeaderAbsolute(const fs::path &absolute_filepath) {
  internal::add_path(absolute_filepath, current_header_files_);
}

void Target::AddHeader(const std::string &relative_filename,
                       const fs::path &relative_to_target_path) {
  env::log_trace(name_, __FUNCTION__);

  // Check Source
  fs::path absolute_filepath =
      target_root_source_dir_ / relative_to_target_path / relative_filename;
  AddHeaderAbsolute(absolute_filepath);
}

void Target::AddHeader(const std::string &relative_filename) {
  AddHeader(relative_filename, "");
}

void Target::GlobHeaders(const fs::path &relative_to_target_path) {
  env::log_trace(name_, __FUNCTION__);

  fs::path absolute_filepath =
      target_root_source_dir_ / relative_to_target_path;

  for (const auto &p : fs::directory_iterator(absolute_filepath)) {
    const bool regular_file = p.is_regular_file();
    if (!regular_file) {
      continue;
    }

    const std::string ext = p.path().extension().string();
    const bool header_match = valid_header_ext_.count(ext) == 1;
    if (header_match) {
      env::log_trace(name_, fmt::format("Added header {}", p.path().string()));
      AddHeaderAbsolute(p.path());
    }
  }
}

// Public
void Target::AddIncludeDir(const fs::path &relative_include_dir,
                           bool glob_headers) {
  env::log_trace(name_, __FUNCTION__);

  const std::string absolute_include_dir =
      (target_root_source_dir_ / relative_include_dir)
          .make_preferred()
          .string();
  current_include_dirs_.insert(absolute_include_dir);

  if (glob_headers) {
    GlobHeaders(relative_include_dir);
  }
}

} // namespace buildcc::base
