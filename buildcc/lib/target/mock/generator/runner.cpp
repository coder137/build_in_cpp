#include "target/generator.h"

#include "expect_generator.h"

namespace buildcc::m {

void GeneratorRunner(FileGenerator &generator) {
  tf::Executor executor(1);
  executor.run(generator.GetTaskflow());
  executor.wait_for_all();
}

} // namespace buildcc::m
