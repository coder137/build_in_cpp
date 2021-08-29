#include "target/generator.h"

#include "command/command.h"

namespace buildcc::base {

void Generator::GenerateTask() {
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
}

} // namespace buildcc::base
