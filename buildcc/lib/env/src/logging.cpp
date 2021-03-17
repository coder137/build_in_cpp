#include "logging.h"

#include "spdlog/spdlog.h"

namespace buildcc::env {

void set_log_pattern(const std::string &pattern) {
  spdlog::set_pattern(pattern);
}

void set_log_level(LogLevel level) {
  spdlog::set_level((spdlog::level::level_enum)level);
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
