#include "logging.h"

#include "spdlog/spdlog.h"

namespace buildcc::env {

void set_log_pattern(std::string_view pattern) {
  spdlog::set_pattern(pattern.data());
}

void set_log_level(LogLevel level) {
  spdlog::set_level((spdlog::level::level_enum)level);
}

void log(LogLevel level, std::string_view tag, std::string_view message) {
  spdlog::log((spdlog::level::level_enum)level, "[{}]: {}", tag, message);
}
void log_trace(std::string_view tag, std::string_view message) {
  log(LogLevel::Trace, tag, message);
}
void log_debug(std::string_view tag, std::string_view message) {
  log(LogLevel::Debug, tag, message);
}
void log_info(std::string_view tag, std::string_view message) {
  log(LogLevel::Info, tag, message);
}
void log_warning(std::string_view tag, std::string_view message) {
  log(LogLevel::Warning, tag, message);
}
void log_critical(std::string_view tag, std::string_view message) {
  log(LogLevel::Critical, tag, message);
}

} // namespace buildcc::env
