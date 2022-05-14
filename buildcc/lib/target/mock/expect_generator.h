#ifndef TARGET_MOCK_EXPECT_GENERATOR_H_
#define TARGET_MOCK_EXPECT_GENERATOR_H_

#include "target/generator.h"

namespace buildcc::m {

/**
 * @brief Runs the generator using Taskflow with 1 thread
 * CppUTest cannot mock with multiple threads
 */
void GeneratorRunner(FileGenerator &generator);

} // namespace buildcc::m

#endif
