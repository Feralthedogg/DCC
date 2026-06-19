#ifndef DCC_WINDOWS_INTERNAL_H
#define DCC_WINDOWS_INTERNAL_H

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#endif
