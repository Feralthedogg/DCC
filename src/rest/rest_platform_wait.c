#include "internal/rest/dcc_rest_runtime_internal.h"

#include <llam/runtime.h>

#include <errno.h>
#include <stdint.h>
#if defined(_WIN32)
#include <windows.h>
#else
#include <limits.h>
#include <time.h>
#endif

void dcc_rest_sleep_ms(uint64_t ms) {
    if (ms == 0) {
        return;
    }
    uint64_t ns = ms > UINT64_MAX / UINT64_C(1000000) ? UINT64_MAX : ms * UINT64_C(1000000);
    if (llam_sleep_ns(ns) == 0) {
        return;
    }

#if defined(_WIN32)
    Sleep(ms > UINT32_MAX ? UINT32_MAX : (DWORD)ms);
#else
    struct timespec req;
    req.tv_sec = (time_t)(ms / 1000U);
    req.tv_nsec = (long)((ms % 1000U) * 1000000U);
    while (nanosleep(&req, &req) != 0 && errno == EINTR) {
    }
#endif
}

uint64_t dcc_rest_timeout_deadline_ns(uint32_t timeout_ms) {
    uint64_t now = llam_now_ns();
    uint64_t delta = (uint64_t)timeout_ms * UINT64_C(1000000);
    return delta > UINT64_MAX - now ? UINT64_MAX : now + delta;
}

#if !defined(_WIN32)
dcc_status_t dcc_rest_status_from_pthread(int rc) {
    if (rc == 0) {
        return DCC_OK;
    }
    return dcc_rest_wait_status_from_errno(rc);
}

static void dcc_rest_timespec_add_ms(struct timespec *ts, uint64_t ms) {
    uint64_t sec = ms / 1000U;
    uint64_t nsec = (ms % 1000U) * UINT64_C(1000000);
    if (sec > (uint64_t)LONG_MAX - (uint64_t)ts->tv_sec) {
        ts->tv_sec = (time_t)LONG_MAX;
        ts->tv_nsec = 999999999L;
        return;
    }
    ts->tv_sec += (time_t)sec;
    ts->tv_nsec += (long)nsec;
    if (ts->tv_nsec >= 1000000000L) {
        ts->tv_sec++;
        ts->tv_nsec -= 1000000000L;
    }
}

dcc_status_t dcc_rest_make_deadline_ms(uint64_t wait_ms, struct timespec *out) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (clock_gettime(CLOCK_REALTIME, out) != 0) {
        return dcc_rest_wait_status_from_errno(errno);
    }
    dcc_rest_timespec_add_ms(out, wait_ms);
    return DCC_OK;
}
#endif
