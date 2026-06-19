#include "internal/events/dcc_events_internal.h"

#include <errno.h>
#if defined(_WIN32)
#include <windows.h>
#else
#include <time.h>
#endif

uint64_t dcc_event_waiter_deadline_ns(uint32_t timeout_ms) {
    uint64_t now = llam_now_ns();
    uint64_t delta = (uint64_t)timeout_ms * UINT64_C(1000000);
    return delta > UINT64_MAX - now ? UINT64_MAX : now + delta;
}

void dcc_event_waiter_sleep_ms(uint32_t ms) {
    if (ms == 0) {
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
