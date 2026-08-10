#include "internal/app/dcc_app_internal.h"

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#error "App internals must establish WIN32_LEAN_AND_MEAN before windows.h"
#endif
#ifndef NOMINMAX
#error "App internals must establish NOMINMAX before windows.h"
#endif

#include <llam/runtime.h>

#if defined(_WINSOCKAPI_) && !defined(_WINSOCK2API_)
#error "App internals included winsock.h before LLAM's winsock2 contract"
#endif
#endif

int main(void) {
    return 0;
}
