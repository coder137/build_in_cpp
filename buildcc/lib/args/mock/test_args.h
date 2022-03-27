#ifndef ARGS_MOCK_TEST_ARGS_H_
#define ARGS_MOCK_TEST_ARGS_H_

#include "args/args.h"

#include <cassert>

namespace buildcc::m {

inline void ArgsDeinit() { Args::Deinit(); }

} // namespace buildcc::m

#endif
