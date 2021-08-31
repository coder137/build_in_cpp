#include "target/generator.h"

#include "command/command.h"

namespace buildcc::base {

void Generator::GenerateTask() {
  pregenerate_cb_();
  Convert();
  const auto generated_files = BuildGenerate();
  if (!dirty_) {
    return;
  }
  // NOTE, info->parallel is not checked
  for (const auto &info : generated_files) {
    for (const auto &command : info->commands) {
      bool success = Command::Execute(command);
      env::assert_fatal(success, fmt::format("{} failed", command));
    }
  }
  Store();
  postgenerate_cb_();
}

} // namespace buildcc::base
