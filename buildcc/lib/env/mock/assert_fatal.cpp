#include "env/assert_fatal.h"

#include <exception>

namespace buildcc::env {

void assert_handle_fatal() { throw std::exception(); }

} // namespace buildcc::env
