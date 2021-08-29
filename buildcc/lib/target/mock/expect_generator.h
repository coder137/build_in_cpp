#ifndef TARGET_MOCK_EXPECT_GENERATOR_H_
#define TARGET_MOCK_EXPECT_GENERATOR_H_

#include "target/generator.h"

namespace buildcc::base::m {

void GeneratorExpect_InputRemoved(unsigned int calls, Generator *generator);
void GeneratorExpect_InputAdded(unsigned int calls, Generator *generator);
void GeneratorExpect_InputUpdated(unsigned int calls, Generator *generator);

void GeneratorExpect_OutputChanged(unsigned int calls, Generator *generator);
void GeneratorExpect_CommandChanged(unsigned int calls, Generator *generator);

} // namespace buildcc::base::m

#endif
