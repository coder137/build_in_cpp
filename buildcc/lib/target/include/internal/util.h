#ifndef TARGET_INCLUDE_INTERNAL_UTIL_H_
#define TARGET_INCLUDE_INTERNAL_UTIL_H_

#include <string>
#include <vector>

#include "internal/path.h"

namespace buildcc::internal {

// System
bool command(const std::vector<std::string> &command_tokens);

// Checks
bool is_previous_paths_different(const path_unordered_set &previous_paths,
                                 const path_unordered_set &current_paths);

// Aggregates
std::string aggregate(const std::vector<std::string> &list);
std::string aggregate(const std::unordered_set<std::string> &list);
std::string aggregate(const buildcc::internal::path_unordered_set &paths);

std::string aggregate_include_dirs(const path_unordered_set &include_dirs);

} // namespace buildcc::internal

#endif
