#include <dcc/hot_reload/canary.h>

#include <string.h>

void dcc_hot_reload_canary_options_init(dcc_hot_reload_canary_options_t *options) {
    if (options != NULL) {
        memset(options, 0, sizeof(*options));
        options->size = sizeof(*options);
        options->canary_percent = 5U;
        options->promote_after_ms = 30000U;
        options->max_error_rate = 0.05;
        options->max_consecutive_failures = 3U;
    }
}

dcc_status_t dcc_hot_reload_canary_options_validate(
    const dcc_hot_reload_canary_options_t *options
) {
    if (options == NULL ||
        options->canary_percent == 0U ||
        options->canary_percent > 100U ||
        options->promote_after_ms == 0U ||
        options->max_error_rate < 0.0 ||
        options->max_error_rate > 1.0 ||
        options->max_consecutive_failures == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

dcc_status_t dcc_hot_reload_canary_start(
    dcc_hot_reload_canary_state_t *state,
    const dcc_hot_reload_canary_options_t *options,
    uint64_t candidate_generation,
    uint64_t now_ms
) {
    if (state == NULL || candidate_generation == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_hot_reload_canary_options_t defaults;
    if (options == NULL) {
        dcc_hot_reload_canary_options_init(&defaults);
        options = &defaults;
    }
    dcc_status_t status = dcc_hot_reload_canary_options_validate(options);
    if (status != DCC_OK) {
        return status;
    }
    memset(state, 0, sizeof(*state));
    state->size = sizeof(*state);
    state->options = *options;
    state->candidate_generation = candidate_generation;
    state->started_at_ms = now_ms;
    state->active = 1U;
    return DCC_OK;
}

void dcc_hot_reload_canary_stop(dcc_hot_reload_canary_state_t *state) {
    if (state != NULL) {
        state->active = 0U;
    }
}

uint8_t dcc_hot_reload_canary_should_route(
    const dcc_hot_reload_canary_state_t *state,
    uint64_t request_hash
) {
    if (state == NULL || !state->active || state->options.canary_percent == 0U) {
        return 0U;
    }
    return (uint8_t)((request_hash % 100U) < state->options.canary_percent);
}

dcc_status_t dcc_hot_reload_canary_record(
    dcc_hot_reload_canary_state_t *state,
    uint8_t success
) {
    if (state == NULL || !state->active) {
        return DCC_ERR_STATE;
    }
    if (state->routed == UINT64_MAX ||
        state->successes == UINT64_MAX ||
        state->failures == UINT64_MAX) {
        return DCC_ERR_NOMEM;
    }
    state->routed++;
    if (success) {
        state->successes++;
        state->consecutive_failures = 0U;
    } else {
        state->failures++;
        if (state->consecutive_failures != UINT32_MAX) {
            state->consecutive_failures++;
        }
    }
    return DCC_OK;
}

dcc_hot_reload_canary_decision_t dcc_hot_reload_canary_decide(
    dcc_hot_reload_canary_state_t *state,
    uint64_t now_ms
) {
    if (state == NULL || !state->active) {
        return DCC_HOT_RELOAD_CANARY_KEEP;
    }
    if (state->consecutive_failures >= state->options.max_consecutive_failures) {
        state->rollback_reason = DCC_HOT_RELOAD_CANARY_ROLLBACK_CONSECUTIVE_FAILURES;
        return DCC_HOT_RELOAD_CANARY_ROLLBACK;
    }
    if (state->routed != 0U) {
        double error_rate = (double)state->failures / (double)state->routed;
        if (error_rate > state->options.max_error_rate) {
            state->rollback_reason = DCC_HOT_RELOAD_CANARY_ROLLBACK_ERROR_RATE;
            return DCC_HOT_RELOAD_CANARY_ROLLBACK;
        }
    }
    if (now_ms >= state->started_at_ms &&
        now_ms - state->started_at_ms >= state->options.promote_after_ms) {
        return DCC_HOT_RELOAD_CANARY_PROMOTE;
    }
    return DCC_HOT_RELOAD_CANARY_KEEP;
}

const char *dcc_hot_reload_canary_decision_string(dcc_hot_reload_canary_decision_t decision) {
    switch (decision) {
        case DCC_HOT_RELOAD_CANARY_KEEP:
            return "keep";
        case DCC_HOT_RELOAD_CANARY_PROMOTE:
            return "promote";
        case DCC_HOT_RELOAD_CANARY_ROLLBACK:
            return "rollback";
        default:
            return "unknown";
    }
}

const char *dcc_hot_reload_canary_rollback_reason_string(
    dcc_hot_reload_canary_rollback_reason_t reason
) {
    switch (reason) {
        case DCC_HOT_RELOAD_CANARY_ROLLBACK_NONE:
            return "none";
        case DCC_HOT_RELOAD_CANARY_ROLLBACK_ERROR_RATE:
            return "error_rate";
        case DCC_HOT_RELOAD_CANARY_ROLLBACK_CONSECUTIVE_FAILURES:
            return "consecutive_failures";
        case DCC_HOT_RELOAD_CANARY_ROLLBACK_HEALTHCHECK_FAILED:
            return "healthcheck_failed";
        default:
            return "unknown";
    }
}
