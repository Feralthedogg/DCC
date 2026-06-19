#include "internal/rest/dcc_rest_firewall_internal.h"

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static uint64_t dcc_firewall_add_ms(uint64_t base, uint64_t delta) {
    return UINT64_MAX - base < delta ? UINT64_MAX : base + delta;
}

dcc_rest_firewall_state_t *dcc_rest_firewall_state_new(
    const dcc_rest_firewall_options_t *options,
    uint8_t attached
) {
    dcc_rest_firewall_options_t effective;
    if (options == NULL) {
        dcc_rest_firewall_options_init(&effective);
        options = &effective;
    }
    if (dcc_rest_firewall_options_validate(options) != DCC_OK) {
        return NULL;
    }

    dcc_rest_firewall_state_t *state = (dcc_rest_firewall_state_t *)calloc(1, sizeof(*state));
    if (state == NULL) {
        return NULL;
    }
    state->size = sizeof(*state);
    state->options = *options;
    state->options.size = sizeof(state->options);
    state->attached = attached != 0U;
    return state;
}

void dcc_rest_firewall_state_free(dcc_rest_firewall_state_t *state) {
    free(state);
}

void dcc_rest_firewall_state_normalize_window(
    dcc_rest_firewall_state_t *state,
    uint64_t now_ms
) {
    if (state == NULL) {
        return;
    }
    if (!state->window_active) {
        state->window_started_at_ms = now_ms;
        state->invalid_request_count = 0;
        state->window_active = 1U;
        return;
    }

    uint64_t window_end = dcc_firewall_add_ms(
        state->window_started_at_ms,
        state->options.invalid_request_window_ms
    );
    if (now_ms >= window_end) {
        state->window_started_at_ms = now_ms;
        state->invalid_request_count = 0;
    }
}

static void dcc_firewall_result_set(
    dcc_rest_firewall_result_t *result,
    dcc_rest_firewall_decision_t decision,
    dcc_rest_firewall_reason_t reason,
    uint64_t delay_ms,
    uint8_t critical
) {
    if (result == NULL) {
        return;
    }
    memset(result, 0, sizeof(*result));
    result->size = sizeof(*result);
    result->decision = decision;
    result->reason = reason;
    result->delay_ms = delay_ms;
    result->critical = critical != 0U;
}

static uint8_t dcc_firewall_soft_exceeded(const dcc_rest_firewall_state_t *state) {
    return state->options.invalid_request_soft_limit != 0U &&
        state->invalid_request_count >= state->options.invalid_request_soft_limit;
}

static uint8_t dcc_firewall_hard_exceeded(const dcc_rest_firewall_state_t *state) {
    return state->invalid_request_count >= state->options.invalid_request_hard_limit;
}

dcc_status_t dcc_rest_firewall_state_check(
    dcc_rest_firewall_state_t *state,
    const dcc_rest_firewall_request_t *request,
    dcc_rest_firewall_result_t *result
) {
    if (state == NULL || request == NULL ||
        request->size < offsetof(dcc_rest_firewall_request_t, critical) + sizeof(request->critical)) {
        return DCC_ERR_INVALID_ARG;
    }

    uint64_t now_ms = request->now_ms;
    dcc_rest_firewall_state_normalize_window(state, now_ms);
    if (state->total_requests != UINT64_MAX) {
        state->total_requests++;
    }

    uint8_t critical = request->critical != 0U ||
        dcc_rest_firewall_request_is_critical(request->method, request->path);

    if (dcc_firewall_hard_exceeded(state)) {
        if (state->options.on_hard_limit == DCC_REST_FIREWALL_HARD_LIMIT_DELAY) {
            if (state->total_delayed_requests != UINT64_MAX) {
                state->total_delayed_requests++;
            }
            dcc_firewall_result_set(
                result,
                DCC_REST_FIREWALL_DELAY,
                DCC_REST_FIREWALL_REASON_HARD_INVALID_REQUEST_LIMIT,
                state->options.soft_limit_delay_ms,
                critical
            );
            return DCC_OK;
        }
        if (state->options.on_hard_limit == DCC_REST_FIREWALL_HARD_LIMIT_REJECT_ALL ||
            !critical) {
            if (state->total_rejected_requests != UINT64_MAX) {
                state->total_rejected_requests++;
            }
            dcc_firewall_result_set(
                result,
                DCC_REST_FIREWALL_REJECT,
                DCC_REST_FIREWALL_REASON_HARD_INVALID_REQUEST_LIMIT,
                0,
                critical
            );
            return DCC_OK;
        }
    }

    if (dcc_firewall_soft_exceeded(state) && state->options.soft_limit_delay_ms != 0U) {
        if (state->total_delayed_requests != UINT64_MAX) {
            state->total_delayed_requests++;
        }
        dcc_firewall_result_set(
            result,
            DCC_REST_FIREWALL_DELAY,
            DCC_REST_FIREWALL_REASON_SOFT_INVALID_REQUEST_LIMIT,
            state->options.soft_limit_delay_ms,
            critical
        );
        return DCC_OK;
    }

    dcc_firewall_result_set(result, DCC_REST_FIREWALL_ALLOW, DCC_REST_FIREWALL_REASON_NONE, 0, critical);
    return DCC_OK;
}

void dcc_rest_firewall_state_record_response(
    dcc_rest_firewall_state_t *state,
    uint16_t status,
    uint64_t now_ms
) {
    if (state == NULL) {
        return;
    }
    dcc_rest_firewall_state_normalize_window(state, now_ms);
    if (!dcc_rest_firewall_status_is_invalid(status)) {
        return;
    }
    if (state->invalid_request_count != SIZE_MAX) {
        state->invalid_request_count++;
    }
    if (state->total_invalid_requests != UINT64_MAX) {
        state->total_invalid_requests++;
    }
}

void dcc_rest_firewall_snapshot_from_state(
    const dcc_rest_firewall_state_t *state,
    uint64_t now_ms,
    dcc_rest_firewall_snapshot_t *snapshot
) {
    if (snapshot == NULL) {
        return;
    }
    memset(snapshot, 0, sizeof(*snapshot));
    snapshot->size = sizeof(*snapshot);
    if (state == NULL) {
        dcc_rest_firewall_options_init(&snapshot->options);
        return;
    }

    snapshot->options = state->options;
    snapshot->options.size = sizeof(snapshot->options);
    snapshot->window_started_at_ms = state->window_active ? state->window_started_at_ms : now_ms;
    snapshot->window_ends_at_ms = dcc_firewall_add_ms(
        snapshot->window_started_at_ms,
        state->options.invalid_request_window_ms
    );
    snapshot->invalid_request_count = state->invalid_request_count;
    snapshot->total_requests = state->total_requests;
    snapshot->total_invalid_requests = state->total_invalid_requests;
    snapshot->total_delayed_requests = state->total_delayed_requests;
    snapshot->total_rejected_requests = state->total_rejected_requests;
    snapshot->soft_limit_exceeded = dcc_firewall_soft_exceeded(state);
    snapshot->hard_limit_exceeded = dcc_firewall_hard_exceeded(state);
    snapshot->attached = state->attached;
}
