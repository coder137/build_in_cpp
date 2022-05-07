#ifndef TARGET_MOCK_EXPECT_CUSTOM_GENERATOR_H_
#define TARGET_MOCK_EXPECT_CUSTOM_GENERATOR_H_

#include "target/custom_generator.h"

namespace buildcc::m {

/**
 * @brief Runs the generator using Taskflow with 1 thread
 * CppUTest cannot mock with multiple threads
 */
void CustomGeneratorRunner(CustomGenerator &custom_generator);

void CustomGeneratorExpect_IdRemoved(unsigned int calls,
                                     CustomGenerator *generator);
void CustomGeneratorExpect_IdAdded(unsigned int calls,
                                   CustomGenerator *generator);
void CustomGeneratorExpect_IdUpdated(unsigned int calls,
                                     CustomGenerator *generator);

} // namespace buildcc::m

#endif
