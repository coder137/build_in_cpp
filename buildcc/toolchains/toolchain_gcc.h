#include "toolchain.h"

namespace buildcc {

class Toolchain_gcc : public base::Toolchain {
public:
  Toolchain_gcc() : Toolchain("gcc", "as", "gcc", "g++", "ar", "ld") {}
  Toolchain_gcc(const Toolchain_gcc &gcc) = delete;
};

} // namespace buildcc
