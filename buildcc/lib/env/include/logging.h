#ifndef ENV_INCLUDE_LOGGING_H_
#define ENV_INCLUDE_LOGGING_H_

#include <string>

namespace buildcc::env {

// NOTE, These numbers are mapped to spdlog::level::level_enum internally
enum class LogLevel {
  Trace = 0,
  Debug = 1,
  Info = 2,
  Warning = 3,
  Critical = 5,
  Error = Critical,
  Fatal = Critical,
};

void set_log_pattern(const std::string_view &pattern);
void set_log_level(LogLevel level);

// Logging functions
void log(LogLevel level, const std::string_view &message,
         const std::string_view &name = "env");
void log_trace(const std::string_view &message,
               const std::string_view &name = "env");
void log_debug(const std::string_view &message,
               const std::string_view &name = "env");
void log_info(const std::string_view &message,
              const std::string_view &name = "env");
void log_warning(const std::string_view &message,
                 const std::string_view &name = "env");
void log_critical(const std::string_view &message,
                  const std::string_view &name = "env");

} // namespace buildcc::env

#endif
