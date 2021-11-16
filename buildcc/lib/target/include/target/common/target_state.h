#ifndef TARGET_COMMON_TARGET_STATE_H_
#define TARGET_COMMON_TARGET_STATE_H_

#include "target/common/target_file_ext.h"

namespace buildcc::base {

struct TargetState {

  void SetSourceState(TargetFileExt file_extension);
  void Lock();

  void ExpectsUnlock() const;
  void ExpectsLock() const;

  bool contains_pch{false};
  bool contains_asm{false};
  bool contains_c{false};
  bool contains_cpp{false};
  bool build{false};
  bool lock{false};
};

} // namespace buildcc::base

#endif
