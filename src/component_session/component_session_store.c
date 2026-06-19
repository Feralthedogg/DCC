#include "internal/component_session/dcc_component_session_internal.h"

#include <stdlib.h>
#include <string.h>

void dcc_component_session_store_init(dcc_component_session_store_t *store) {
    if (store != NULL) {
        memset(store, 0, sizeof(*store));
        store->size = sizeof(*store);
    }
}

void dcc_component_session_store_deinit(dcc_component_session_store_t *store) {
    if (store == NULL) {
        return;
    }
    dcc_component_session_store_state_t *state =
        (dcc_component_session_store_state_t *)store->state;
    if (state != NULL) {
        for (size_t i = 0; i < state->count; ++i) {
            dcc_component_session_state_deinit(&state->sessions[i]);
        }
        free(state->sessions);
        free(state);
    }
    memset(store, 0, sizeof(*store));
}

static dcc_status_t dcc_component_session_store_ensure(
    dcc_component_session_store_t *store,
    dcc_component_session_store_state_t **out_state
) {
    if (store == NULL || out_state == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_component_session_store_state_t *state =
        (dcc_component_session_store_state_t *)store->state;
    if (state == NULL) {
        state = (dcc_component_session_store_state_t *)calloc(1, sizeof(*state));
        if (state == NULL) {
            return DCC_ERR_NOMEM;
        }
        store->size = sizeof(*store);
        store->state = state;
    }
    *out_state = state;
    return DCC_OK;
}

static dcc_status_t dcc_component_session_store_grow(
    dcc_component_session_store_state_t *state
) {
    if (state->count < state->cap) {
        return DCC_OK;
    }
    size_t next_cap = 8U;
    if (state->cap != 0U) {
        if (state->cap > SIZE_MAX / 2U) {
            return DCC_ERR_NOMEM;
        }
        next_cap = state->cap * 2U;
    }
    if (next_cap > SIZE_MAX / sizeof(*state->sessions)) {
        return DCC_ERR_NOMEM;
    }
    dcc_component_session_state_t *next =
        (dcc_component_session_state_t *)realloc(state->sessions, next_cap * sizeof(*state->sessions));
    if (next == NULL) {
        return DCC_ERR_NOMEM;
    }
    memset(next + state->cap, 0, (next_cap - state->cap) * sizeof(*next));
    state->sessions = next;
    state->cap = next_cap;
    return DCC_OK;
}

dcc_status_t dcc_component_session_store_add(
    dcc_component_session_store_t *store,
    const dcc_component_session_t *session
) {
    dcc_component_session_state_t *src = dcc_component_session_state(session);
    if (src == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_component_session_store_state_t *state = NULL;
    dcc_status_t status = dcc_component_session_store_ensure(store, &state);
    if (status != DCC_OK) {
        return status;
    }

    dcc_component_session_state_t copy;
    status = dcc_component_session_state_copy(&copy, src);
    if (status != DCC_OK) {
        return status;
    }

    for (size_t i = 0; i < state->count; ++i) {
        if (strcmp(state->sessions[i].session_id, src->session_id) == 0) {
            dcc_component_session_state_deinit(&state->sessions[i]);
            state->sessions[i] = copy;
            return DCC_OK;
        }
    }

    status = dcc_component_session_store_grow(state);
    if (status != DCC_OK) {
        dcc_component_session_state_deinit(&copy);
        return status;
    }
    state->sessions[state->count++] = copy;
    return DCC_OK;
}

dcc_status_t dcc_component_session_store_verify(
    const dcc_component_session_store_t *store,
    const dcc_component_session_check_t *check,
    dcc_component_session_result_t *result
) {
    if (store == NULL || check == NULL || check->custom_id == NULL || result == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    memset(result, 0, sizeof(*result));
    result->size = sizeof(*result);

    dcc_component_session_parts_t parts;
    dcc_status_t status = dcc_component_session_parse_custom_id(check->custom_id, &parts);
    if (status != DCC_OK) {
        result->status = DCC_COMPONENT_SESSION_VERIFY_BAD_FORMAT;
        return DCC_OK;
    }
    memcpy(result->session_id, parts.session_id, sizeof(result->session_id));
    memcpy(result->action, parts.action, sizeof(result->action));
    memcpy(result->nonce, parts.nonce, sizeof(result->nonce));

    const dcc_component_session_store_state_t *state =
        (const dcc_component_session_store_state_t *)store->state;
    if (state == NULL) {
        result->status = DCC_COMPONENT_SESSION_VERIFY_NOT_FOUND;
        return DCC_OK;
    }
    result->status = DCC_COMPONENT_SESSION_VERIFY_NOT_FOUND;
    for (size_t i = 0; i < state->count; ++i) {
        dcc_component_session_verify_status_t match =
            dcc_component_session_verify_parts(&state->sessions[i], &parts, check);
        if (match != DCC_COMPONENT_SESSION_VERIFY_NOT_FOUND) {
            result->status = match;
            result->expires_at_ms = state->sessions[i].expires_at_ms;
            break;
        }
    }
    return DCC_OK;
}

size_t dcc_component_session_store_sweep(
    dcc_component_session_store_t *store,
    uint64_t now_ms
) {
    if (store == NULL || store->state == NULL) {
        return 0;
    }
    dcc_component_session_store_state_t *state =
        (dcc_component_session_store_state_t *)store->state;
    size_t removed = 0;
    size_t i = 0;
    while (i < state->count) {
        if (now_ms > state->sessions[i].expires_at_ms) {
            dcc_component_session_state_deinit(&state->sessions[i]);
            if (i + 1U < state->count) {
                state->sessions[i] = state->sessions[state->count - 1U];
                memset(&state->sessions[state->count - 1U], 0, sizeof(state->sessions[state->count - 1U]));
            }
            --state->count;
            ++removed;
            continue;
        }
        ++i;
    }
    return removed;
}

size_t dcc_component_session_store_count(const dcc_component_session_store_t *store) {
    const dcc_component_session_store_state_t *state =
        store != NULL ? (const dcc_component_session_store_state_t *)store->state : NULL;
    return state != NULL ? state->count : 0U;
}
