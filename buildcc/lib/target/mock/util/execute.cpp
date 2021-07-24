#include "target/command.h"

#include "CppUTestExt/MockSupport.h"

namespace buildcc::internal {

static constexpr const char *const EXECUTE_FUNCTION = "execute";

// command
bool Command::Execute(const std::string &command) {
  (void)command;
  return mock().actualCall(EXECUTE_FUNCTION).returnBoolValue();
}

namespace m {

void CommandExpect_Execute(unsigned int calls, bool expectation) {
  mock().expectNCalls(calls, EXECUTE_FUNCTION).andReturnValue(expectation);
}

} // namespace m

} // namespace buildcc::internal
