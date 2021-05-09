#ifndef TOOLCHAINS_TOOLCHAIN_GCC_H_
#define TOOLCHAINS_TOOLCHAIN_GCC_H_

#include "toolchain.h"

namespace buildcc {

class Toolchain_gcc : public base::Toolchain {
public:
  Toolchain_gcc() : Toolchain("gcc", "as", "gcc", "g++", "ar", "ld") {}
  Toolchain_gcc(const Toolchain_gcc &gcc) = delete;
};

} // namespace buildcc

#endif
