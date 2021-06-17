#include "internal/util.h"

#include "CppUTestExt/MockSupport.h"

namespace buildcc::internal {

static constexpr const char *const COMMAND_FUNCTION = "command";

// command
bool command(const std::string &command) {
  (void)command;
  return mock().actualCall(COMMAND_FUNCTION).returnBoolValue();
}

namespace m {

void Expect_command(unsigned int calls, bool expectation) {
  mock().expectNCalls(calls, COMMAND_FUNCTION).andReturnValue(expectation);
}

} // namespace m

} // namespace buildcc::internal
