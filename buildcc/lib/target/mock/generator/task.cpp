#include "target/generator.h"

#include "env/util.h"

#include "command/command.h"

namespace buildcc::base {

void Generator::GenerateTask() {
  Convert();
  BuildGenerate();

  if (!dirty_) {
    return;
  }

  // NOTE, info->parallel is not checked
  // Run the command
  for (const auto &command : current_commands_) {
    bool success = Command::Execute(command);
    env::assert_fatal(success, fmt::format("{} failed", command));
  }

  // Generate the output file
  for (const auto &output_file : current_output_files_) {
    std::string file = output_file.string();
    bool success = env::SaveFile(file.c_str(), "", false);
    env::assert_fatal(success, fmt::format("{} failed", file));
  }

  if (dirty_) {
    Store();
  }
}

} // namespace buildcc::base
