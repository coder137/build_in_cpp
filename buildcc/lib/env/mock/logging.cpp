#include "logging.h"

// Stubs
namespace buildcc::env {

// Called by user only
void set_log_pattern(std::string_view pattern) { (void)pattern; }
void set_log_level(LogLevel level) { (void)level; }

// Called by user and program
// Not needed to be mocked
void log(LogLevel level, std::string_view message, std::string_view name) {
  (void)level;
  (void)message;
  (void)name;
}
void log_trace(std::string_view message, std::string_view name) {
  (void)message;
  (void)name;
}
void log_debug(std::string_view message, std::string_view name) {
  (void)message;
  (void)name;
}
void log_info(std::string_view message, std::string_view name) {
  (void)message;
  (void)name;
}
void log_warning(std::string_view message, std::string_view name) {
  (void)message;
  (void)name;
}
void log_critical(std::string_view message, std::string_view name) {
  (void)message;
  (void)name;
}

} // namespace buildcc::env
