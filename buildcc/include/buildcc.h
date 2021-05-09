#ifndef BUILDCC_INCLUDE_BUILDCC_H_
#define BUILDCC_INCLUDE_BUILDCC_H_

// clang-format off

// Core persistent environment
#include "env.h"
#include "logging.h"

// Base
#include "toolchain.h"
#include "target.h"

// Specialized Toolchain
#include "toolchain_gcc.h"
#include "toolchain_msvc.h"

// TODO, Add more specialized toolchains here

// Target

// GCC
#include "executable_target_gcc.h"
#include "static_target_gcc.h"
#include "dynamic_target_gcc.h"

// MSVC
#include "executable_target_msvc.h"
#include "static_target_msvc.h"
#include "dynamic_target_msvc.h"

// TODO, Add more specialized targets here

#endif
