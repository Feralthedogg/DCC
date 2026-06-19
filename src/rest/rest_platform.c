#include "internal/rest/dcc_rest_runtime_internal.h"

#include <llam/runtime.h>

#include <errno.h>
#include <stdint.h>

uint64_t dcc_rest_now_ms(void) {
    return llam_now_ns() / UINT64_C(1000000);
}

uint64_t dcc_rest_seconds_to_ms(double seconds) {
    if (seconds <= 0.0) {
        return 0;
    }
    double ms = seconds * 1000.0;
    if (ms >= (double)UINT64_MAX) {
        return UINT64_MAX;
    }
    return (uint64_t)(ms + 0.999);
}

uint64_t dcc_rest_add_ms(uint64_t base, uint64_t delta) {
    if (delta > UINT64_MAX - base) {
        return UINT64_MAX;
    }
    return base + delta;
}

dcc_status_t dcc_rest_wait_status_from_errno(int err) {
    switch (err) {
        case ETIMEDOUT:
        case EAGAIN:
            return DCC_ERR_TIMEOUT;
        case ECANCELED:
        case EPIPE:
            return DCC_ERR_CANCELED;
        case ENOMEM:
            return DCC_ERR_NOMEM;
        case EINVAL:
            return DCC_ERR_INVALID_ARG;
        case ENOTSUP:
        case EXDEV:
            return DCC_ERR_STATE;
        default:
            return DCC_ERR_RUNTIME;
    }
}
