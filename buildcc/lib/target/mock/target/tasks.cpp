#include "target/target.h"

namespace buildcc::base {

void CompilePch::Task() { BuildCompile(); }

void CompileObject::Task() {
  std::vector<fs::path> source_files;
  std::vector<fs::path> dummy_source_files;

  BuildObjectCompile(source_files, dummy_source_files);

  for (const auto &s : source_files) {
    bool success = Command::Execute(GetObjectData(s).command);
    env::assert_fatal(success, "Could not compile source");
  }
}

void Target::TargetTask() { BuildTargetLink(); }

} // namespace buildcc::base
