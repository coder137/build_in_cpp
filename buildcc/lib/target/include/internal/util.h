#ifndef TARGET_INCLUDE_INTERNAL_UTIL_H_
#define TARGET_INCLUDE_INTERNAL_UTIL_H_

#include <string>
#include <vector>

namespace buildcc::internal {

// System
bool command(const std::vector<std::string> &command_tokens);

// Aggregates
std::string
aggregate_compiled_sources(const std::vector<std::string> &compiled_sources);

} // namespace buildcc::internal

#endif
