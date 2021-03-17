#ifndef BUILDCC_INCLUDE_ENV_ENV_H_
#define BUILDCC_INCLUDE_ENV_ENV_H_

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

// Basic Initialization
void init(const std::string &build_root_location,
          const std::string &build_intermediate_location);
void set_log_level(LogLevel level);

// Getters
bool is_init(void);
const std::string &get_build_root_location();
const std::string &get_build_intermediate_location();

// Logging
void log(LogLevel level, const std::string &message,
         const std::string &name = "env");
void log_trace(const std::string &message, const std::string &name = "env");
void log_debug(const std::string &message, const std::string &name = "env");
void log_info(const std::string &message, const std::string &name = "env");
void log_warning(const std::string &message, const std::string &name = "env");
void log_critical(const std::string &message, const std::string &name = "env");

} // namespace buildcc::env

#endif
