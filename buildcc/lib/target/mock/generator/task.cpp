#include "target/generator.h"

#include "command/command.h"

namespace buildcc::base {

void Generator::GenerateTask(tf::Taskflow &tf) {
  (void)tf;

  pregenerate_cb_();
  Convert();
  std::vector<const internal::GenInfo *> generated_files;
  std::vector<const internal::GenInfo *> dummy_generated_files;
  BuildGenerate(generated_files, dummy_generated_files);

  // NOTE, info->parallel is not checked
  for (const auto &info : generated_files) {
    for (const auto &command : info->commands) {
      bool success = Command::Execute(command);
      env::assert_fatal(success, fmt::format("{} failed", command));
    }
  }
  (void)dummy_generated_files;

  if (dirty_) {
    Store();
    postgenerate_cb_();
  }
}

} // namespace buildcc::base
