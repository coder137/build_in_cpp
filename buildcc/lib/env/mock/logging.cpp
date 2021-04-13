#include "logging.h"

// Stubs
namespace buildcc::env {

// Called by user only
void set_log_pattern(const std::string_view &pattern) { (void)pattern; }
void set_log_level(LogLevel level) { (void)level; }

// Called by user and program
// Not needed to be mocked
void log(LogLevel level, const std::string_view &message,
         const std::string_view &name) {
  (void)level;
  (void)message;
  (void)name;
}
void log_trace(const std::string_view &message, const std::string_view &name) {
  (void)message;
  (void)name;
}
void log_debug(const std::string_view &message, const std::string_view &name) {
  (void)message;
  (void)name;
}
void log_info(const std::string_view &message, const std::string_view &name) {
  (void)message;
  (void)name;
}
void log_warning(const std::string_view &message,
                 const std::string_view &name) {
  (void)message;
  (void)name;
}
void log_critical(const std::string_view &message,
                  const std::string_view &name) {
  (void)message;
  (void)name;
}

} // namespace buildcc::env
