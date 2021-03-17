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
  spdlog::set_pattern("%^[%l]%$ %v");
  set_log_level(LogLevel::Info);
}

void set_log_level(LogLevel level) {
  spdlog::set_level((spdlog::level::level_enum)level);
}

bool is_init(void) { return init_; }
const std::string &get_build_root_location() { return build_root_location_; }
const std::string &get_build_intermediate_location() {
  return build_intermediate_location_;
}

void log(LogLevel level, const std::string &message, const std::string &name) {
  spdlog::log((spdlog::level::level_enum)level, "[{}]: {}", name, message);
}
void log_trace(const std::string &message, const std::string &name) {
  log(LogLevel::Trace, message, name);
}
void log_debug(const std::string &message, const std::string &name) {
  log(LogLevel::Debug, message, name);
}
void log_info(const std::string &message, const std::string &name) {
  log(LogLevel::Info, message, name);
}
void log_warning(const std::string &message, const std::string &name) {
  log(LogLevel::Warning, message, name);
}
void log_critical(const std::string &message, const std::string &name) {
  log(LogLevel::Critical, message, name);
}

} // namespace buildcc::env
