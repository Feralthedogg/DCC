#include "internal/component_session/dcc_component_session_internal.h"

#include "internal/json/dcc_json.h"
#include "internal/rest/dcc_rest_buffer_internal.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int dcc_component_session_hex_value(char ch) {
    if (ch >= '0' && ch <= '9') {
        return ch - '0';
    }
    if (ch >= 'a' && ch <= 'f') {
        return ch - 'a' + 10;
    }
    if (ch >= 'A' && ch <= 'F') {
        return ch - 'A' + 10;
    }
    return -1;
}

static dcc_status_t dcc_component_session_append_json_u64_string(
    dcc_rest_buffer_t *buffer,
    uint64_t value
) {
    char text[32];
    int n = snprintf(text, sizeof(text), "%llu", (unsigned long long)value);
    if (n <= 0 || (size_t)n >= sizeof(text)) {
        return DCC_ERR_RUNTIME;
    }
    return dcc_rest_buffer_append_json_string(buffer, text);
}

static dcc_status_t dcc_component_session_append_secret_hex(
    dcc_rest_buffer_t *buffer,
    const unsigned char *secret,
    size_t secret_len
) {
    static const char hex[] = "0123456789abcdef";
    if (buffer == NULL || secret == NULL || secret_len == 0U) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_rest_buffer_append_cstr(buffer, "\"");
    if (status != DCC_OK) {
        return status;
    }
    for (size_t i = 0U; i < secret_len; ++i) {
        char pair[2];
        pair[0] = hex[secret[i] >> 4U];
        pair[1] = hex[secret[i] & 0x0FU];
        status = dcc_rest_buffer_append(buffer, pair, sizeof(pair));
        if (status != DCC_OK) {
            return status;
        }
    }
    return dcc_rest_buffer_append_cstr(buffer, "\"");
}

static dcc_status_t dcc_component_session_append_session_json(
    dcc_rest_buffer_t *buffer,
    const dcc_component_session_state_t *session
) {
    if (buffer == NULL || session == NULL || session->secret == NULL ||
        session->secret_len == 0U) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_rest_buffer_append_cstr(buffer, "{\"session_id\":");
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_json_string(buffer, session->session_id);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(buffer, ",\"nonce\":");
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_json_string(buffer, session->nonce);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(buffer, ",\"secret_hex\":");
    }
    if (status == DCC_OK) {
        status = dcc_component_session_append_secret_hex(
            buffer,
            session->secret,
            session->secret_len
        );
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(buffer, ",\"expires_at_ms\":");
    }
    if (status == DCC_OK) {
        status = dcc_component_session_append_json_u64_string(buffer, session->expires_at_ms);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(buffer, ",\"user_id\":");
    }
    if (status == DCC_OK) {
        status = dcc_component_session_append_json_u64_string(buffer, session->user_id);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(buffer, ",\"channel_id\":");
    }
    if (status == DCC_OK) {
        status = dcc_component_session_append_json_u64_string(buffer, session->channel_id);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(buffer, ",\"guild_id\":");
    }
    if (status == DCC_OK) {
        status = dcc_component_session_append_json_u64_string(buffer, session->guild_id);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(
            buffer,
            session->lock_user ? ",\"lock_user\":true" : ",\"lock_user\":false"
        );
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(
            buffer,
            session->lock_channel ? ",\"lock_channel\":true" : ",\"lock_channel\":false"
        );
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(
            buffer,
            session->lock_guild ? ",\"lock_guild\":true}" : ",\"lock_guild\":false}"
        );
    }
    return status;
}

dcc_status_t dcc_component_session_store_export_json(
    const dcc_component_session_store_t *store,
    char **out_json,
    size_t *out_len
) {
    if (store == NULL || out_json == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_json = NULL;
    if (out_len != NULL) {
        *out_len = 0U;
    }

    const dcc_component_session_store_state_t *state =
        (const dcc_component_session_store_state_t *)store->state;
    dcc_rest_buffer_t buffer;
    memset(&buffer, 0, sizeof(buffer));

    dcc_status_t status = dcc_rest_buffer_append_cstr(
        &buffer,
        "{\"version\":1,\"sessions\":["
    );
    for (size_t i = 0U; status == DCC_OK && state != NULL && i < state->count; ++i) {
        if (i != 0U) {
            status = dcc_rest_buffer_append_cstr(&buffer, ",");
        }
        if (status == DCC_OK) {
            status = dcc_component_session_append_session_json(&buffer, &state->sessions[i]);
        }
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&buffer, "]}");
    }
    if (status != DCC_OK) {
        dcc_rest_buffer_deinit(&buffer);
        return status;
    }

    *out_json = buffer.data;
    if (out_len != NULL) {
        *out_len = buffer.len;
    }
    return DCC_OK;
}

static int dcc_component_session_parse_u64_text(const char *text, uint64_t *out) {
    if (text == NULL || text[0] == '\0' || out == NULL) {
        return 0;
    }
    uint64_t value = 0U;
    for (const char *p = text; *p != '\0'; ++p) {
        if (*p < '0' || *p > '9') {
            return 0;
        }
        uint64_t digit = (uint64_t)(*p - '0');
        if (value > (UINT64_MAX - digit) / 10U) {
            return 0;
        }
        value = value * 10U + digit;
    }
    *out = value;
    return 1;
}

static dcc_status_t dcc_component_session_json_u64_required(
    const dcc_json_t *object,
    const char *key,
    uint64_t *out
) {
    const dcc_json_t *value = dcc_json_object_get(object, key);
    if (dcc_json_u64(value, out) == 0) {
        return DCC_OK;
    }
    return dcc_component_session_parse_u64_text(dcc_json_string(value), out)
        ? DCC_OK
        : DCC_ERR_JSON;
}

static dcc_status_t dcc_component_session_json_bool_required(
    const dcc_json_t *object,
    const char *key,
    uint8_t *out
) {
    int value = 0;
    if (dcc_json_bool(dcc_json_object_get(object, key), &value) != 0 || out == NULL) {
        return DCC_ERR_JSON;
    }
    *out = value ? 1U : 0U;
    return DCC_OK;
}

static dcc_status_t dcc_component_session_decode_secret_hex(
    const char *hex,
    unsigned char **out_secret,
    size_t *out_secret_len
) {
    if (hex == NULL || out_secret == NULL || out_secret_len == NULL) {
        return DCC_ERR_JSON;
    }
    *out_secret = NULL;
    *out_secret_len = 0U;

    size_t len = strlen(hex);
    if (len == 0U || (len % 2U) != 0U || len / 2U > (size_t)INT_MAX) {
        return DCC_ERR_JSON;
    }
    unsigned char *secret = (unsigned char *)malloc(len / 2U);
    if (secret == NULL) {
        return DCC_ERR_NOMEM;
    }
    for (size_t i = 0U; i < len; i += 2U) {
        int high = dcc_component_session_hex_value(hex[i]);
        int low = dcc_component_session_hex_value(hex[i + 1U]);
        if (high < 0 || low < 0) {
            free(secret);
            return DCC_ERR_JSON;
        }
        secret[i / 2U] = (unsigned char)((high << 4) | low);
    }
    *out_secret = secret;
    *out_secret_len = len / 2U;
    return DCC_OK;
}

static dcc_status_t dcc_component_session_import_one(
    dcc_component_session_store_t *store,
    const dcc_json_t *json
) {
    if (store == NULL || json == NULL || dcc_json_typeof(json) != DCC_JSON_OBJECT) {
        return DCC_ERR_JSON;
    }

    const char *session_id = dcc_json_string(dcc_json_object_get(json, "session_id"));
    const char *nonce = dcc_json_string(dcc_json_object_get(json, "nonce"));
    const char *secret_hex = dcc_json_string(dcc_json_object_get(json, "secret_hex"));
    if (!dcc_component_session_token_valid(session_id, DCC_COMPONENT_SESSION_ID_MAX) ||
        !dcc_component_session_token_valid(nonce, DCC_COMPONENT_SESSION_NONCE_MAX)) {
        return DCC_ERR_JSON;
    }

    dcc_component_session_state_t *state =
        (dcc_component_session_state_t *)calloc(1U, sizeof(*state));
    if (state == NULL) {
        return DCC_ERR_NOMEM;
    }
    memcpy(state->session_id, session_id, strlen(session_id) + 1U);
    memcpy(state->nonce, nonce, strlen(nonce) + 1U);

    dcc_status_t status = dcc_component_session_decode_secret_hex(
        secret_hex,
        &state->secret,
        &state->secret_len
    );
    if (status == DCC_OK) {
        status = dcc_component_session_json_u64_required(
            json,
            "expires_at_ms",
            &state->expires_at_ms
        );
    }
    if (status == DCC_OK) {
        uint64_t user_id = 0U;
        status = dcc_component_session_json_u64_required(json, "user_id", &user_id);
        state->user_id = (dcc_snowflake_t)user_id;
    }
    if (status == DCC_OK) {
        uint64_t channel_id = 0U;
        status = dcc_component_session_json_u64_required(json, "channel_id", &channel_id);
        state->channel_id = (dcc_snowflake_t)channel_id;
    }
    if (status == DCC_OK) {
        uint64_t guild_id = 0U;
        status = dcc_component_session_json_u64_required(json, "guild_id", &guild_id);
        state->guild_id = (dcc_snowflake_t)guild_id;
    }
    if (status == DCC_OK) {
        status = dcc_component_session_json_bool_required(json, "lock_user", &state->lock_user);
    }
    if (status == DCC_OK) {
        status = dcc_component_session_json_bool_required(json, "lock_channel", &state->lock_channel);
    }
    if (status == DCC_OK) {
        status = dcc_component_session_json_bool_required(json, "lock_guild", &state->lock_guild);
    }

    dcc_component_session_t session;
    memset(&session, 0, sizeof(session));
    session.size = sizeof(session);
    session.state = state;
    if (status == DCC_OK) {
        status = dcc_component_session_store_add(store, &session);
    }
    dcc_component_session_state_deinit(state);
    free(state);
    return status;
}

dcc_status_t dcc_component_session_store_import_json(
    dcc_component_session_store_t *store,
    const char *json,
    size_t json_len
) {
    if (store == NULL || json == NULL || json_len == 0U) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_json_t *root = NULL;
    dcc_status_t status = dcc_json_parse(json, json_len, &root);
    if (status != DCC_OK) {
        return status;
    }
    if (root == NULL || dcc_json_typeof(root) != DCC_JSON_OBJECT) {
        dcc_json_free(root);
        return DCC_ERR_JSON;
    }

    uint64_t version = 0U;
    status = dcc_component_session_json_u64_required(root, "version", &version);
    const dcc_json_t *sessions = dcc_json_object_get(root, "sessions");
    if (status == DCC_OK &&
        (version != 1U || sessions == NULL || dcc_json_typeof(sessions) != DCC_JSON_ARRAY)) {
        status = DCC_ERR_JSON;
    }
    for (size_t i = 0U; status == DCC_OK && i < dcc_json_array_size(sessions); ++i) {
        status = dcc_component_session_import_one(store, dcc_json_array_get(sessions, i));
    }

    dcc_json_free(root);
    return status;
}

void dcc_component_session_store_json_free(char *json) {
    free(json);
}
