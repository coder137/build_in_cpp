#ifndef ENV_INCLUDE_ASSERT_FATAL_H_
#define ENV_INCLUDE_ASSERT_FATAL_H_

#include <string>

#include "logging.h"

namespace buildcc::env {

class assert_exception : public std::exception {
public:
  assert_exception(std::string_view message) : message_(message) {}

private:
  virtual const char *what() const throw() { return message_.data(); }

private:
  std::string_view message_;
};

inline void assert_fatal(bool expression, std::string_view message) {
  if (!expression) {
    buildcc::env::log_critical("assert", message);
    throw assert_exception(message);
  }
}

} // namespace buildcc::env

#endif
