#ifndef TARGET_INCLUDE_INTERNAL_UTIL_H_
#define TARGET_INCLUDE_INTERNAL_UTIL_H_

#include <string>
#include <vector>

#include "internal/path.h"

namespace buildcc::internal {

// System
bool command(const std::vector<std::string> &command_tokens);

// Additions
/**
 * @brief Existing path is stored inside stored_paths
 * Returns false if path is stored
 * Throws exception if path does not exist
 *
 * @param path
 * @param stored_paths
 * @return true
 * @return false
 */
bool add_path(const fs::path &path, path_unordered_set &stored_paths);

/**
 * @brief Attach quotes to filesystem::path::string if space detected
 * Returns same string if no space detected.
 * Meant to be called after filesystem::path::make_preferred
 *
 * @param str
 * @return std::string
 */
std::string quote(const std::string &str);

// Checks
bool is_previous_paths_different(const path_unordered_set &previous_paths,
                                 const path_unordered_set &current_paths);

// Aggregates
std::string aggregate(const std::vector<std::string> &list);
std::string aggregate(const std::unordered_set<std::string> &list);
std::string aggregate(const buildcc::internal::path_unordered_set &paths);

} // namespace buildcc::internal

#endif
