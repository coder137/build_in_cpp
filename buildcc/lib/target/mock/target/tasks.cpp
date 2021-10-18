#include "target/target.h"

namespace buildcc::base {

void Target::PchTask() { BuildPch(); }

void Target::CompileTask() {
  std::vector<fs::path> source_files;
  std::vector<fs::path> dummy_source_files;

  BuildCompile(source_files, dummy_source_files);

  for (const auto &s : source_files) {
    bool success = Command::Execute(GetCompileCommand(s));
    env::assert_fatal(success, "Could not compile source");
  }
}

void Target::LinkTask() { BuildLink(); }

} // namespace buildcc::base
