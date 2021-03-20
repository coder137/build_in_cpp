#ifndef TARGET_INCLUDE_TARGET_TYPE_H_
#define TARGET_INCLUDE_TARGET_TYPE_H_

namespace buildcc {

enum class TargetType {
  Executable,
  StaticLibrary,
  DynamicLibrary,
};

}

#endif
