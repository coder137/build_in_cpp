#include "env/command.h"

#include "expect_command.h"

#include "CppUTestExt/MockSupport.h"

namespace buildcc::env {

static constexpr const char *const EXECUTE_FUNCTION = "execute";
static constexpr const char *const STDOUT_DATA_STRING = "stdout_data";
static constexpr const char *const STDERR_DATA_STRING = "stderr_data";

// command
bool Command::Execute(const std::string &command,
                      const std::optional<fs::path> &working_directory,
                      std::vector<std::string> *stdout_data,
                      std::vector<std::string> *stderr_data) {
  (void)command;
  (void)working_directory;
  auto &actualcall = mock().actualCall(EXECUTE_FUNCTION);
  if (stdout_data != nullptr) {
    actualcall.withOutputParameterOfType(
        TEST_VECTOR_STRING_TYPE, STDOUT_DATA_STRING, (void *)stdout_data);
  }
  if (stderr_data != nullptr) {
    actualcall.withOutputParameterOfType(
        TEST_VECTOR_STRING_TYPE, STDERR_DATA_STRING, (void *)stderr_data);
  }
  return actualcall.returnBoolValue();
}

namespace m {

void CommandExpect_Execute(unsigned int calls, bool expectation,
                           std::vector<std::string> *stdout_data,
                           std::vector<std::string> *stderr_data) {
  auto &expectedcall = mock().expectNCalls(calls, EXECUTE_FUNCTION);
  if (stdout_data != nullptr) {
    expectedcall.withOutputParameterOfTypeReturning(
        TEST_VECTOR_STRING_TYPE, STDOUT_DATA_STRING, (void *)stdout_data);
  }
  if (stderr_data != nullptr) {
    expectedcall.withOutputParameterOfTypeReturning(
        TEST_VECTOR_STRING_TYPE, STDERR_DATA_STRING, (void *)stderr_data);
  }
  expectedcall.andReturnValue(expectation);
}

} // namespace m

} // namespace buildcc::env
