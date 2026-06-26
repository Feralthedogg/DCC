#include "internal/client/dcc_client_state_internal.h"
#include "internal/dcc_core_internal.h"
#include "internal/gateway/dcc_gateway_ws_control_internal.h"
#include "internal/rest/dcc_rest_buffer_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"
#include "internal/ws/dcc_ws.h"

#include <stdio.h>
#include <string.h>

static dcc_status_t dcc_gateway_send_text_payload(
    dcc_client_t *client,
    const char *payload,
    size_t payload_len,
    const char *error_message
) {
    if (client == NULL || payload == NULL || payload_len == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    if (dcc_gateway_should_stop(client) ||
        !client->runtime.initialized ||
        !atomic_load_explicit(&client->started, memory_order_acquire) ||
        !atomic_load_explicit(&client->gateway_task_running, memory_order_acquire)) {
        return DCC_ERR_STATE;
    }

    dcc_gateway_control_lock(client);
    dcc_ws_t *ws = (dcc_ws_t *)client->gateway_current_ws;
    dcc_status_t status = ws != NULL ? dcc_ws_retain(ws) : DCC_ERR_STATE;
    dcc_gateway_control_unlock(client);
    if (status == DCC_OK) {
        status = dcc_ws_send_text(ws, payload, payload_len);
        dcc_ws_destroy(ws);
    }
    if (status != DCC_OK) {
        dcc_set_error(client, error_message);
    }
    return status;
}

static dcc_status_t dcc_gateway_buffer_append_i64(dcc_rest_buffer_t *body, int64_t value) {
    char text[32];
    int n = snprintf(text, sizeof(text), "%lld", (long long)value);
    if (n <= 0 || (size_t)n >= sizeof(text)) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_buffer_append(body, text, (size_t)n);
}

dcc_status_t dcc_client_request_guild_members(
    dcc_client_t *client,
    const dcc_gateway_request_guild_members_options_t *options
) {
    if (options == NULL || options->size < sizeof(*options) || options->guild_id == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    if (options->nonce != NULL && strlen(options->nonce) > 32U) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{\"op\":8,\"d\":{");
    int first = 1;
    if (status == DCC_OK) {
        status = dcc_rest_json_append_u64_string_member(&body, &first, "guild_id", options->guild_id);
    }
    if (status == DCC_OK && options->user_id_count > 0) {
        if (options->user_ids == NULL) {
            status = DCC_ERR_INVALID_ARG;
        } else {
            status = dcc_rest_json_append_member_key(&body, &first, "user_ids");
        }
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_snowflake_json_string_array(
                &body,
                options->user_ids,
                options->user_id_count
            );
        }
    } else if (status == DCC_OK) {
        status = dcc_rest_json_append_string_member(&body, &first, "query", options->query != NULL ? options->query : "");
        if (status == DCC_OK) {
            status = dcc_rest_json_append_u64_member(&body, &first, "limit", options->limit);
        }
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_bool_member(&body, &first, "presences", options->presences);
    }
    if (status == DCC_OK && options->nonce != NULL && options->nonce[0] != '\0') {
        status = dcc_rest_json_append_string_member(&body, &first, "nonce", options->nonce);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, "}}");
    }
    if (status == DCC_OK) {
        status = dcc_gateway_send_text_payload(
            client,
            body.data,
            body.len,
            "gateway request guild members send failed"
        );
    }
    dcc_rest_buffer_deinit(&body);
    return status;
}

dcc_status_t dcc_client_request_soundboard_sounds(
    dcc_client_t *client,
    const dcc_snowflake_t *guild_ids,
    size_t guild_id_count
) {
    if (guild_ids == NULL || guild_id_count == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{\"op\":31,\"d\":{\"guild_ids\":");
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_snowflake_json_string_array(&body, guild_ids, guild_id_count);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, "}}");
    }
    if (status == DCC_OK) {
        status = dcc_gateway_send_text_payload(
            client,
            body.data,
            body.len,
            "gateway request soundboard sounds send failed"
        );
    }
    dcc_rest_buffer_deinit(&body);
    return status;
}

dcc_status_t dcc_client_request_channel_info(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char * const *fields,
    size_t field_count
) {
    if (guild_id == 0 || fields == NULL || field_count == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{\"op\":43,\"d\":{");
    int first = 1;
    if (status == DCC_OK) {
        status = dcc_rest_json_append_u64_string_member(&body, &first, "guild_id", guild_id);
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_member_key(&body, &first, "fields");
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_json_string_array(&body, fields, field_count);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, "}}");
    }
    if (status == DCC_OK) {
        status = dcc_gateway_send_text_payload(
            client,
            body.data,
            body.len,
            "gateway request channel info send failed"
        );
    }
    dcc_rest_buffer_deinit(&body);
    return status;
}

dcc_status_t dcc_client_update_presence_raw(dcc_client_t *client, const char *presence_json) {
    if (presence_json == NULL || presence_json[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{\"op\":3,\"d\":");
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, presence_json);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, "}");
    }
    if (status == DCC_OK) {
        status = dcc_gateway_send_text_payload(client, body.data, body.len, "gateway presence update send failed");
    }
    dcc_rest_buffer_deinit(&body);
    return status;
}

dcc_status_t dcc_client_update_presence(
    dcc_client_t *client,
    const dcc_gateway_presence_update_t *presence
) {
    if (presence == NULL || presence->size < sizeof(*presence) ||
        presence->status == NULL || presence->status[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }
    const char *activities = presence->activities_json != NULL ? presence->activities_json : "[]";

    dcc_rest_buffer_t body = {0};
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
    int first = 1;
    if (status == DCC_OK) {
        status = dcc_rest_json_append_member_key(&body, &first, "since");
    }
    if (status == DCC_OK) {
        status = presence->has_since ?
            dcc_gateway_buffer_append_i64(&body, presence->since_ms) :
            dcc_rest_buffer_append_cstr(&body, "null");
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_member_key(&body, &first, "activities");
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, activities);
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_string_member(&body, &first, "status", presence->status);
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_bool_member(&body, &first, "afk", presence->afk);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, "}");
    }
    if (status == DCC_OK) {
        status = dcc_client_update_presence_raw(client, body.data);
    }
    dcc_rest_buffer_deinit(&body);
    return status;
}
