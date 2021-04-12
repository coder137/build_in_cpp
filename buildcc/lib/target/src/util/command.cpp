#include "internal/util.h"

#include "env.h"

namespace buildcc::internal {

// command
bool command(const std::vector<std::string> &command_tokens) {
  std::string command{""};
  for (const auto &t : command_tokens) {
    command += t + " ";
  }
  buildcc::env::log_debug(command, "system");
  return system(command.c_str()) == 0;
}

} // namespace buildcc::internal
