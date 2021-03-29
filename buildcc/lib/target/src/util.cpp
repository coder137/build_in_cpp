#include "internal/util.h"

#include "env.h"

namespace {

std::string aggregate_strlist(const std::vector<std::string> &list) {
  std::string files = "";
  for (const auto &l : list) {
    files += l + " ";
  }
  return files;
}

} // namespace

namespace buildcc::internal {

bool command(const std::vector<std::string> &command_tokens) {
  std::string command{""};
  for (const auto &t : command_tokens) {
    command += t + " ";
  }
  buildcc::env::log_debug(command, "system");
  return system(command.c_str()) == 0;
}

std::string
aggregate_compiled_sources(const std::vector<std::string> &compiled_sources) {
  return aggregate_strlist(compiled_sources);
}

} // namespace buildcc::internal
