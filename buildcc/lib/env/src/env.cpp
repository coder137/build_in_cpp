#include "env.h"

#include "logging.h"

namespace {

fs::path root_dir_{""};
fs::path build_dir_{""};
bool init_ = false;

} // namespace

namespace buildcc::env {

void init(const fs::path &project_root, const fs::path &project_build_dir) {
  // State
  root_dir_ = project_root;
  build_dir_ = project_build_dir;
  root_dir_.make_preferred();
  build_dir_.make_preferred();

  init_ = true;

  // Logging
  set_log_pattern("%^[%l]%$ %v");
  set_log_level(LogLevel::Info);
}

void deinit() {
  root_dir_ = "";
  build_dir_ = "";
  init_ = false;
}

bool is_init(void) { return init_; }
const fs::path &get_project_root_dir() { return root_dir_; }
const fs::path &get_project_build_dir() { return build_dir_; }

} // namespace buildcc::env
