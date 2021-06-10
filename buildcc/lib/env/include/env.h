#ifndef ENV_INCLUDE_ENV_H_
#define ENV_INCLUDE_ENV_H_

#include <string>

#include <filesystem>

namespace fs = std::filesystem;

namespace buildcc::env {

/**
 * @brief Initialize project environment
 *
 * @param project_root_dir Root directory for source files
 * @param project_build_dir Directory for intermediate build files
 */
void init(const fs::path &project_root_dir, const fs::path &project_build_dir);
void deinit();

// Getters
bool is_init();
const fs::path &get_project_root_dir();
const fs::path &get_project_build_dir();

} // namespace buildcc::env

#endif
