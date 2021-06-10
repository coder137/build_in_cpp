#ifndef ENV_INCLUDE_ASSERT_FATAL_H_
#define ENV_INCLUDE_ASSERT_FATAL_H_

#include <string>

#include "logging.h"

namespace buildcc::env {

class assert_exception : public std::exception {
public:
  assert_exception(const char *const message) : message_(message) {}

private:
  virtual const char *what() const throw() { return message_; }

private:
  const char *const message_;
};

inline void assert_fatal(bool expression, const std::string &message) {
  if (!expression) {
    buildcc::env::log_critical("assert", message);
    throw assert_exception(message.c_str());
  }
}

} // namespace buildcc::env

#endif
