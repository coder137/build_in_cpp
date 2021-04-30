#include "env.h"

#include "logging.h"

namespace {

fs::path project_root_{""};
fs::path intermediate_build_dir_{""};
bool init_ = false;

} // namespace

namespace buildcc::env {

void init(const fs::path &project_root,
          const fs::path &intermediate_build_dir) {
  // State
  project_root_ = project_root;
  intermediate_build_dir_ = intermediate_build_dir;
  project_root_.make_preferred();
  intermediate_build_dir_.make_preferred();

  init_ = true;

  // Logging
  set_log_pattern("%^[%l]%$ %v");
  set_log_level(LogLevel::Info);
}

void deinit() {
  project_root_ = "";
  intermediate_build_dir_ = "";
  init_ = false;
}

bool is_init(void) { return init_; }
const fs::path &get_project_root() { return project_root_; }
const fs::path &get_intermediate_build_dir() { return intermediate_build_dir_; }

} // namespace buildcc::env
