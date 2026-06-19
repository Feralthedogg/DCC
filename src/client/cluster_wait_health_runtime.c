#include "internal/client/dcc_cluster_internal.h"

#include <llam/runtime.h>

#include <errno.h>
#if defined(_WIN32)
#include <windows.h>
#else
#include <time.h>
#endif

uint64_t dcc_cluster_health_wait_now_ms(void) {
    return llam_now_ns() / UINT64_C(1000000);
}

void dcc_cluster_health_wait_sleep_ms(uint32_t ms) {
    if (ms == 0U) {
        return;
    }
    if (llam_sleep_ns((uint64_t)ms * UINT64_C(1000000)) == 0) {
        return;
    }
#if defined(_WIN32)
    Sleep((DWORD)ms);
#else
    struct timespec req;
    req.tv_sec = (time_t)(ms / 1000U);
    req.tv_nsec = (long)((ms % 1000U) * 1000000U);
    while (nanosleep(&req, &req) != 0 && errno == EINTR) {
    }
#endif
}
