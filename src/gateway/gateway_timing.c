#include "internal/gateway/dcc_gateway_timing_internal.h"

#include <llam/runtime.h>

uint32_t dcc_gateway_now_jitter(uint32_t max_ms, uint64_t salt) {
    if (max_ms == 0) {
        return 0;
    }
    uint64_t value = llam_now_ns() ^ (salt * UINT64_C(11400714819323198485));
    value ^= value >> 33U;
    value *= UINT64_C(0xff51afd7ed558ccd);
    value ^= value >> 33U;
    return (uint32_t)(value % (uint64_t)max_ms);
}

uint64_t dcc_gateway_now_ms(void) {
    return llam_now_ns() / 1000000ULL;
}
