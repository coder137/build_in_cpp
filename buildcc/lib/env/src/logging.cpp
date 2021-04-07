#include "logging.h"

#include "spdlog/spdlog.h"

namespace buildcc::env {

void set_log_pattern(const std::string_view &pattern) {
  spdlog::set_pattern(pattern.data());
}

void set_log_level(LogLevel level) {
  spdlog::set_level((spdlog::level::level_enum)level);
}

void log(LogLevel level, const std::string_view &message,
         const std::string_view &name) {
  spdlog::log((spdlog::level::level_enum)level, "[{}]: {}", name, message);
}
void log_trace(const std::string_view &message, const std::string_view &name) {
  log(LogLevel::Trace, message, name);
}
void log_debug(const std::string_view &message, const std::string_view &name) {
  log(LogLevel::Debug, message, name);
}
void log_info(const std::string_view &message, const std::string_view &name) {
  log(LogLevel::Info, message, name);
}
void log_warning(const std::string_view &message,
                 const std::string_view &name) {
  log(LogLevel::Warning, message, name);
}
void log_critical(const std::string_view &message,
                  const std::string_view &name) {
  log(LogLevel::Critical, message, name);
}

} // namespace buildcc::env
