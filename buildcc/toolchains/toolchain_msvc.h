#ifndef TOOLCHAINS_TOOLCHAIN_MSVC_H_
#define TOOLCHAINS_TOOLCHAIN_MSVC_H_

#include "toolchain.h"

namespace buildcc {

class Toolchain_msvc : public base::Toolchain {
public:
  Toolchain_msvc() : Toolchain("msvc", "cl", "cl", "cl", "lib", "link") {}
  Toolchain_msvc(const Toolchain_msvc &gcc) = delete;
};

} // namespace buildcc

#endif
