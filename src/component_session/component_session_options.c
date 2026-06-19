#include "internal/component_session/dcc_component_session_internal.h"

#include <string.h>

void dcc_component_session_options_init(dcc_component_session_options_t *options) {
    if (options != NULL) {
        memset(options, 0, sizeof(*options));
        options->size = sizeof(*options);
        options->ttl_ms = 15U * 60U * 1000U;
    }
}

dcc_status_t dcc_component_session_options_set_secret(
    dcc_component_session_options_t *options,
    const void *secret,
    size_t secret_len
) {
    if (options == NULL || secret == NULL || secret_len == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    options->secret = secret;
    options->secret_len = secret_len;
    return DCC_OK;
}

dcc_status_t dcc_component_session_options_set_ids(
    dcc_component_session_options_t *options,
    const char *session_id,
    const char *nonce
) {
    if (options == NULL ||
        !dcc_component_session_token_valid(session_id, DCC_COMPONENT_SESSION_ID_MAX) ||
        !dcc_component_session_token_valid(nonce, DCC_COMPONENT_SESSION_NONCE_MAX)) {
        return DCC_ERR_INVALID_ARG;
    }
    options->session_id = session_id;
    options->nonce = nonce;
    return DCC_OK;
}

dcc_status_t dcc_component_session_options_set_ttl(
    dcc_component_session_options_t *options,
    uint64_t now_ms,
    uint64_t ttl_ms
) {
    if (options == NULL || ttl_ms == 0U || now_ms > UINT64_MAX - ttl_ms) {
        return DCC_ERR_INVALID_ARG;
    }
    options->now_ms = now_ms;
    options->ttl_ms = ttl_ms;
    return DCC_OK;
}

dcc_status_t dcc_component_session_options_lock_user(
    dcc_component_session_options_t *options,
    dcc_snowflake_t user_id
) {
    if (options == NULL || user_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    options->user_id = user_id;
    options->lock_user = 1U;
    return DCC_OK;
}

dcc_status_t dcc_component_session_options_lock_channel(
    dcc_component_session_options_t *options,
    dcc_snowflake_t channel_id
) {
    if (options == NULL || channel_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    options->channel_id = channel_id;
    options->lock_channel = 1U;
    return DCC_OK;
}

dcc_status_t dcc_component_session_options_lock_guild(
    dcc_component_session_options_t *options,
    dcc_snowflake_t guild_id
) {
    if (options == NULL || guild_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    options->guild_id = guild_id;
    options->lock_guild = 1U;
    return DCC_OK;
}
