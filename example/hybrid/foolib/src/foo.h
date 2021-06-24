#pragma once

#ifdef _WIN32
#include <windows.h>
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

EXPORT void vFoo();
EXPORT int iFoo();
EXPORT float fFoo(int integer);
