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

/**
 * @brief Set the log pattern object
 *
 * @param pattern Uses spdlog pattern
 */
void set_log_pattern(std::string_view pattern);
void set_log_level(LogLevel level);

// Logging functions
void log(LogLevel level, std::string_view tag, std::string_view message);
void log_trace(std::string_view tag, std::string_view message);
void log_debug(std::string_view tag, std::string_view message);
void log_info(std::string_view tag, std::string_view message);
void log_warning(std::string_view tag, std::string_view message);
void log_critical(std::string_view tag, std::string_view message);

} // namespace buildcc::env

#endif
