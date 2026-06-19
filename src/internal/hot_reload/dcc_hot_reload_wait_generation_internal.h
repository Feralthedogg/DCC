#ifndef DCC_HOT_RELOAD_WAIT_GENERATION_INTERNAL_H
#define DCC_HOT_RELOAD_WAIT_GENERATION_INTERNAL_H

#include <dcc/hot_reload.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_hot_reload_wait_for_generation(
    dcc_hot_reload_t *hot_reload,
    uint64_t generation,
    uint32_t timeout_ms
);
dcc_status_t dcc_hot_reload_wait_until_loaded(
    dcc_hot_reload_t *hot_reload,
    uint32_t timeout_ms,
    dcc_hot_reload_health_snapshot_t *out
);

#ifdef __cplusplus
}
#endif

#endif
