#include "internal/util.h"

#include "logging.h"

namespace buildcc::internal {

// command
bool command(const std::vector<std::string> &command_tokens) {
  std::string command = aggregate(command_tokens);
  buildcc::env::log_debug(command, "system");
  return system(command.c_str()) == 0;
}

} // namespace buildcc::internal
