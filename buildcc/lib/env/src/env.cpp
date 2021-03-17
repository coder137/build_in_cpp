#include "env.h"

#include "spdlog/spdlog.h"

namespace {

std::string build_root_location_{""};
std::string build_intermediate_location_{""};
bool init_ = false;

} // namespace

namespace buildcc::env {

void init(const std::string &build_root_location,
          const std::string &build_intermediate_location) {
  // State
  build_root_location_ = build_root_location;
  build_intermediate_location_ = build_intermediate_location;
  init_ = true;

  // Logging
  set_log_pattern("%^[%l]%$ %v");
  set_log_level(LogLevel::Info);
}

bool is_init(void) { return init_; }
const std::string &get_build_root_location() { return build_root_location_; }
const std::string &get_build_intermediate_location() {
  return build_intermediate_location_;
}

} // namespace buildcc::env
