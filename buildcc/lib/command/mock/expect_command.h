#ifndef COMMAND_MOCK_EXPECT_COMMAND_H_
#define COMMAND_MOCK_EXPECT_COMMAND_H_

#include <string>
#include <vector>

namespace buildcc::m {

void CommandExpect_Execute(unsigned int calls, bool expectation,
                           std::vector<std::string> *stdout_data = nullptr,
                           std::vector<std::string> *stderr_data = nullptr);

} // namespace buildcc::m

#endif
