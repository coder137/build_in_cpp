#include "env.h"

namespace {

std::string project_root_{""};
std::string intermediate_build_dir_{""};
bool init_ = false;

} // namespace

namespace buildcc::env {

void init(const std::string &project_root,
          const std::string &intermediate_build_dir) {
  // State
  project_root_ = project_root;
  intermediate_build_dir_ = intermediate_build_dir;
  init_ = true;

  // Logging
  set_log_pattern("%^[%l]%$ %v");
  set_log_level(LogLevel::Info);
}

bool is_init(void) { return init_; }
const std::string &get_project_root() { return project_root_; }
const std::string &get_intermediate_build_dir() {
  return intermediate_build_dir_;
}

} // namespace buildcc::env
