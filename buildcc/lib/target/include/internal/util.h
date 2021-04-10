#ifndef TARGET_INCLUDE_INTERNAL_UTIL_H_
#define TARGET_INCLUDE_INTERNAL_UTIL_H_

#include <string>
#include <vector>

#include "internal/path.h"

namespace buildcc::internal {

// System
bool command(const std::vector<std::string> &command_tokens);

// Aggregates
std::string
aggregate_compiled_sources(const std::vector<std::string> &compiled_sources);
std::string aggregate_link_flags(const std::vector<std::string> &link_flags);

std::string aggregate_lib_deps(const path_unordered_set &lib_deps);

} // namespace buildcc::internal

#endif
