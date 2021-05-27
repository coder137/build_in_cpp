#ifndef BUILDCC_INCLUDE_REGISTER_H_
#define BUILDCC_INCLUDE_REGISTER_H_

#include <functional>

#include "args.h"

namespace buildcc {

class Register {
public:
  Register(const Args &args) : args_(args) {}

  void Env();
  void Clean(std::function<void(void)> clean_cb);

private:
  const Args &args_;
};

} // namespace buildcc

#endif
