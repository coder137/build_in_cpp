#ifndef ENV_INCLUDE_ENV_H_
#define ENV_INCLUDE_ENV_H_

#include <string>

#include "logging.h"

namespace buildcc::env {

// Basic Initialization
void init(const std::string &build_root_location,
          const std::string &build_intermediate_location);

// Getters
bool is_init(void);
const std::string &get_build_root_location();
const std::string &get_build_intermediate_location();

} // namespace buildcc::env

#endif
