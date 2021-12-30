#ifndef ENV_MOCK_EXPECT_COMMAND_H_
#define ENV_MOCK_EXPECT_COMMAND_H_

#include <string>
#include <vector>

constexpr const char *const TEST_VECTOR_STRING_TYPE = "vector_string";

namespace buildcc::env::m {

/**
 * @brief `Command::Execute` expectation API
 *
 * @param calls Number of times the actual `Command::Execute` API is called
 * @param expectation Return value of the actual `Command::Execute` API
 * @param stdout_data Data passed into stdout_data is redirected into the
 * actual `Command::Execute` API to check for expectations. See
 * `VectorStringCopier`
 * @param stderr_data Data passed into stderr_data is redirected into the actual
 * `Command::Execute` API to check for expectations. See `VectorStringCopier`
 */
void CommandExpect_Execute(unsigned int calls, bool expectation,
                           std::vector<std::string> *stdout_data = nullptr,
                           std::vector<std::string> *stderr_data = nullptr);

} // namespace buildcc::env::m

#endif
