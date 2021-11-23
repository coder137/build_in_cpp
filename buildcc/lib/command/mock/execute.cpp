#include "command/command.h"

#include "CppUTestExt/MockSupport.h"

namespace buildcc {

static constexpr const char *const EXECUTE_FUNCTION = "execute";

// command
bool Command::Execute(const std::string &command,
                      const std::optional<fs::path> &working_directory,
                      std::vector<std::string> *stdout_data,
                      std::vector<std::string> *stderr_data) {
  (void)command;
  (void)working_directory;
  (void)stdout_data;
  (void)stderr_data;
  return mock().actualCall(EXECUTE_FUNCTION).returnBoolValue();
}

namespace m {

void CommandExpect_Execute(unsigned int calls, bool expectation) {
  mock().expectNCalls(calls, EXECUTE_FUNCTION).andReturnValue(expectation);
}

} // namespace m

} // namespace buildcc
