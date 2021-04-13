#include "internal/util.h"

#include "CppUTestExt/MockSupport.h"

namespace buildcc::internal {

// command
bool command(const std::vector<std::string> &command_tokens) {
  return mock().actualCall("command").returnBoolValue();
}

} // namespace buildcc::internal
