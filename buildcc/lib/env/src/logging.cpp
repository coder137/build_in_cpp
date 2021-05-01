#include "logging.h"

#include "spdlog/spdlog.h"

namespace buildcc::env {

void set_log_pattern(std::string_view pattern) {
  spdlog::set_pattern(pattern.data());
}

void set_log_level(LogLevel level) {
  spdlog::set_level((spdlog::level::level_enum)level);
}

void log(LogLevel level, std::string_view message, std::string_view name) {
  spdlog::log((spdlog::level::level_enum)level, "[{}]: {}", name, message);
}
void log_trace(std::string_view message, std::string_view name) {
  log(LogLevel::Trace, message, name);
}
void log_debug(std::string_view message, std::string_view name) {
  log(LogLevel::Debug, message, name);
}
void log_info(std::string_view message, std::string_view name) {
  log(LogLevel::Info, message, name);
}
void log_warning(std::string_view message, std::string_view name) {
  log(LogLevel::Warning, message, name);
}
void log_critical(std::string_view message, std::string_view name) {
  log(LogLevel::Critical, message, name);
}

} // namespace buildcc::env
