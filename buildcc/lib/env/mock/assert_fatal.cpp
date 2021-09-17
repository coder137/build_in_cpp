#include "env/assert_fatal.h"

#include <exception>

namespace buildcc::env {

[[noreturn]] void assert_handle_fatal() { throw std::exception(); }

} // namespace buildcc::env
