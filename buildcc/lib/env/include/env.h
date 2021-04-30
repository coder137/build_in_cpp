#ifndef ENV_INCLUDE_ENV_H_
#define ENV_INCLUDE_ENV_H_

#include <string>

#include <filesystem>

namespace fs = std::filesystem;

namespace buildcc::env {

// Basic Initialization
void init(const fs::path &project_root, const fs::path &intermediate_build_dir);
void deinit();

// Getters
bool is_init();
const fs::path &get_project_root();
const fs::path &get_intermediate_build_dir();

} // namespace buildcc::env

#endif
