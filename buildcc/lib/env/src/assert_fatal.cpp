#include "env/assert_fatal.h"

#include <exception>

namespace buildcc::env {

[[noreturn]] void assert_handle_fatal() { std::terminate(); }

} // namespace buildcc::env
