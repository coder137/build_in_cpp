#ifndef ENV_MOCK_EXPECT_COMMAND_H_
#define ENV_MOCK_EXPECT_COMMAND_H_

#include <string>
#include <vector>

constexpr const char *const TEST_VECTOR_STRING_TYPE = "vector_string";

namespace buildcc::env::m {

void CommandExpect_Execute(unsigned int calls, bool expectation,
                           std::vector<std::string> *stdout_data = nullptr,
                           std::vector<std::string> *stderr_data = nullptr);

} // namespace buildcc::env::m

#endif
