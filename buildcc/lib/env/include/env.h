#ifndef ENV_INCLUDE_ENV_H_
#define ENV_INCLUDE_ENV_H_

#include <string>

namespace buildcc::env {

// Basic Initialization
void init(const std::string &project_root,
          const std::string &intermediate_build_dir);
void deinit();

// Getters
bool is_init();
const std::string &get_project_root();
const std::string &get_intermediate_build_dir();

} // namespace buildcc::env

#endif
