#ifndef DCC_HOT_RELOAD_CANARY_H
#define DCC_HOT_RELOAD_CANARY_H

#include <dcc/error.h>
#include <dcc/export.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum dcc_hot_reload_canary_decision {
    DCC_HOT_RELOAD_CANARY_KEEP = 0,
    DCC_HOT_RELOAD_CANARY_PROMOTE,
    DCC_HOT_RELOAD_CANARY_ROLLBACK
} dcc_hot_reload_canary_decision_t;

typedef enum dcc_hot_reload_canary_rollback_reason {
    DCC_HOT_RELOAD_CANARY_ROLLBACK_NONE = 0,
    DCC_HOT_RELOAD_CANARY_ROLLBACK_ERROR_RATE,
    DCC_HOT_RELOAD_CANARY_ROLLBACK_CONSECUTIVE_FAILURES,
    DCC_HOT_RELOAD_CANARY_ROLLBACK_HEALTHCHECK_FAILED
} dcc_hot_reload_canary_rollback_reason_t;

typedef struct dcc_hot_reload_canary_options {
    size_t size;
    uint8_t canary_percent;
    uint32_t promote_after_ms;
    double max_error_rate;
    uint32_t max_consecutive_failures;
} dcc_hot_reload_canary_options_t;

typedef struct dcc_hot_reload_canary_state {
    size_t size;
    dcc_hot_reload_canary_options_t options;
    uint64_t candidate_generation;
    uint64_t started_at_ms;
    uint64_t routed;
    uint64_t successes;
    uint64_t failures;
    uint32_t consecutive_failures;
    dcc_hot_reload_canary_rollback_reason_t rollback_reason;
    uint8_t active;
} dcc_hot_reload_canary_state_t;

DCC_API void dcc_hot_reload_canary_options_init(dcc_hot_reload_canary_options_t *options);
DCC_API dcc_status_t dcc_hot_reload_canary_options_validate(
    const dcc_hot_reload_canary_options_t *options
);
DCC_API dcc_status_t dcc_hot_reload_canary_start(
    dcc_hot_reload_canary_state_t *state,
    const dcc_hot_reload_canary_options_t *options,
    uint64_t candidate_generation,
    uint64_t now_ms
);
DCC_API void dcc_hot_reload_canary_stop(dcc_hot_reload_canary_state_t *state);
DCC_API uint8_t dcc_hot_reload_canary_should_route(
    const dcc_hot_reload_canary_state_t *state,
    uint64_t request_hash
);
DCC_API dcc_status_t dcc_hot_reload_canary_record(
    dcc_hot_reload_canary_state_t *state,
    uint8_t success
);
DCC_API dcc_hot_reload_canary_decision_t dcc_hot_reload_canary_decide(
    dcc_hot_reload_canary_state_t *state,
    uint64_t now_ms
);
DCC_API const char *dcc_hot_reload_canary_decision_string(dcc_hot_reload_canary_decision_t decision);
DCC_API const char *dcc_hot_reload_canary_rollback_reason_string(
    dcc_hot_reload_canary_rollback_reason_t reason
);

#ifdef __cplusplus
}
#endif

#endif
