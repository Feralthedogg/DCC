#include "internal/component_session/dcc_component_session_internal.h"

#include <stdlib.h>
#include <string.h>

dcc_component_session_state_t *dcc_component_session_state(
    const dcc_component_session_t *session
) {
    return session != NULL ? (dcc_component_session_state_t *)session->state : NULL;
}

void dcc_component_session_state_deinit(dcc_component_session_state_t *state) {
    if (state == NULL) {
        return;
    }
    free(state->secret);
    for (size_t i = 0; i < state->owned_custom_id_count; ++i) {
        free(state->owned_custom_ids[i]);
    }
    free(state->owned_custom_ids);
    memset(state, 0, sizeof(*state));
}

dcc_status_t dcc_component_session_state_copy(
    dcc_component_session_state_t *dst,
    const dcc_component_session_state_t *src
) {
    if (dst == NULL || src == NULL || src->secret == NULL || src->secret_len == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    memset(dst, 0, sizeof(*dst));
    *dst = *src;
    dst->owned_custom_ids = NULL;
    dst->owned_custom_id_count = 0;
    dst->owned_custom_id_cap = 0;
    dst->secret = (unsigned char *)malloc(src->secret_len);
    if (dst->secret == NULL) {
        memset(dst, 0, sizeof(*dst));
        return DCC_ERR_NOMEM;
    }
    memcpy(dst->secret, src->secret, src->secret_len);
    return DCC_OK;
}

dcc_status_t dcc_component_session_state_adopt_id(
    dcc_component_session_state_t *state,
    char *custom_id
) {
    if (state == NULL || custom_id == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (state->owned_custom_id_count == state->owned_custom_id_cap) {
        size_t next_cap = 4U;
        if (state->owned_custom_id_cap != 0U) {
            if (state->owned_custom_id_cap > SIZE_MAX / 2U) {
                return DCC_ERR_NOMEM;
            }
            next_cap = state->owned_custom_id_cap * 2U;
        }
        if (next_cap > SIZE_MAX / sizeof(*state->owned_custom_ids)) {
            return DCC_ERR_NOMEM;
        }
        char **next = (char **)realloc(state->owned_custom_ids, next_cap * sizeof(*state->owned_custom_ids));
        if (next == NULL) {
            return DCC_ERR_NOMEM;
        }
        state->owned_custom_ids = next;
        state->owned_custom_id_cap = next_cap;
    }
    state->owned_custom_ids[state->owned_custom_id_count++] = custom_id;
    return DCC_OK;
}

dcc_status_t dcc_component_session_create(
    const dcc_component_session_options_t *options,
    dcc_component_session_t *out
) {
    if (options == NULL || out == NULL || options->secret == NULL || options->secret_len == 0U ||
        options->ttl_ms == 0U || options->now_ms > UINT64_MAX - options->ttl_ms) {
        return DCC_ERR_INVALID_ARG;
    }
    if ((options->session_id != NULL &&
         !dcc_component_session_token_valid(options->session_id, DCC_COMPONENT_SESSION_ID_MAX)) ||
        (options->nonce != NULL &&
         !dcc_component_session_token_valid(options->nonce, DCC_COMPONENT_SESSION_NONCE_MAX))) {
        return DCC_ERR_INVALID_ARG;
    }
    memset(out, 0, sizeof(*out));
    dcc_component_session_state_t *state =
        (dcc_component_session_state_t *)calloc(1, sizeof(*state));
    if (state == NULL) {
        return DCC_ERR_NOMEM;
    }
    state->secret = (unsigned char *)malloc(options->secret_len);
    if (state->secret == NULL) {
        free(state);
        return DCC_ERR_NOMEM;
    }
    memcpy(state->secret, options->secret, options->secret_len);
    state->secret_len = options->secret_len;
    state->expires_at_ms = options->now_ms + options->ttl_ms;
    state->user_id = options->user_id;
    state->channel_id = options->channel_id;
    state->guild_id = options->guild_id;
    state->lock_user = options->lock_user;
    state->lock_channel = options->lock_channel;
    state->lock_guild = options->lock_guild;

    dcc_status_t status = DCC_OK;
    if (options->session_id != NULL) {
        memcpy(state->session_id, options->session_id, strlen(options->session_id) + 1U);
    } else {
        status = dcc_component_session_make_random_hex(state->session_id, DCC_COMPONENT_SESSION_ID_MAX);
    }
    if (status == DCC_OK) {
        if (options->nonce != NULL) {
            memcpy(state->nonce, options->nonce, strlen(options->nonce) + 1U);
        } else {
            status = dcc_component_session_make_random_hex(state->nonce, DCC_COMPONENT_SESSION_NONCE_MAX);
        }
    }
    if (status != DCC_OK) {
        dcc_component_session_state_deinit(state);
        free(state);
        return status;
    }

    out->size = sizeof(*out);
    out->state = state;
    return DCC_OK;
}

void dcc_component_session_deinit(dcc_component_session_t *session) {
    if (session == NULL) {
        return;
    }
    dcc_component_session_state_t *state = dcc_component_session_state(session);
    if (state != NULL) {
        dcc_component_session_state_deinit(state);
        free(state);
    }
    memset(session, 0, sizeof(*session));
}
