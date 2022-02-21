#ifndef TARGET_MOCK_TEST_TARGET_UTIL_H_
#define TARGET_MOCK_TEST_TARGET_UTIL_H_

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace buildcc::m {

inline void blocking_sleep(int seconds) {
#ifdef _WIN32
  Sleep(seconds * 1000);
#else
  sleep(seconds);
#endif
}

} // namespace buildcc::m

#endif
