#include "internal/dcc_core_internal.h"
#include "internal/rest/dcc_rest_capture_internal.h"
#include "internal/rest/dcc_rest_direct_messages_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

#include <stdio.h>
#include <string.h>

dcc_status_t dcc_rest_create_direct_message(
    dcc_client_t *client,
    dcc_snowflake_t user_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (client == NULL || user_id == 0 || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_snowflake_t channel_id = dcc_client_get_dm_channel(client, user_id);
    if (channel_id != 0) {
        return dcc_rest_create_message(client, channel_id, json_body, cb, user_data);
    }

    char create_body[64];
    int create_len = snprintf(create_body, sizeof(create_body), "{\"recipient_id\":\"%llu\"}", (unsigned long long)user_id);
    if (create_len < 0 || (size_t)create_len >= sizeof(create_body)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_captured_response_t captured;
    memset(&captured, 0, sizeof(captured));
    dcc_status_t status = dcc_rest_create_dm_channel(client, create_body, dcc_rest_capture_cb, &captured);
    if (status != DCC_OK) {
        dcc_rest_captured_response_deinit(&captured);
        return status;
    }
    if (captured.copy_error != DCC_OK) {
        dcc_rest_captured_response_deinit(&captured);
        return captured.copy_error;
    }
    if (!captured.called) {
        dcc_rest_captured_response_deinit(&captured);
        return DCC_ERR_STATE;
    }

    if (captured.error != DCC_OK || captured.status < 200U || captured.status >= 300U) {
        dcc_status_t response_error = captured.error != DCC_OK ? captured.error : DCC_ERR_DISCORD;
        dcc_rest_forward_captured_response(client, &captured, response_error, cb, user_data);
        dcc_rest_captured_response_deinit(&captured);
        return DCC_OK;
    }

    status = dcc_rest_parse_channel_id_from_json(captured.body, captured.body_len, &channel_id);
    if (status != DCC_OK) {
        dcc_set_error(client, dcc_status_string(status));
        dcc_rest_forward_captured_response(client, &captured, status, cb, user_data);
        dcc_rest_captured_response_deinit(&captured);
        return status;
    }

    (void)dcc_client_set_dm_channel(client, user_id, channel_id);
    dcc_rest_captured_response_deinit(&captured);
    return dcc_rest_create_message(client, channel_id, json_body, cb, user_data);
}

dcc_status_t dcc_rest_create_direct_message_builder(
    dcc_client_t *client,
    dcc_snowflake_t user_id,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
) {
    char *json = NULL;
    dcc_status_t status = dcc_message_builder_build_json(message, &json);
    if (status == DCC_OK) {
        status = dcc_rest_create_direct_message(client, user_id, json, cb, user_data);
    }
    dcc_message_builder_json_free(json);
    return status;
}
