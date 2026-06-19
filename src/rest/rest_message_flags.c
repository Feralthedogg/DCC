#include "internal/rest/dcc_rest_buffer_internal.h"

#include <dcc/rest/messages/edit.h>

dcc_status_t dcc_rest_edit_message_flags(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_edit_message(client, channel_id, message_id, json_body, cb, user_data);
}

dcc_status_t dcc_rest_edit_message_flags_params(
    dcc_client_t *client,
    const dcc_message_flags_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (params == NULL || params->size < sizeof(*params) || params->channel_id == 0 || params->message_id == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{\"flags\":");
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_u64_text(&body, params->flags);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, "}");
    }
    if (status == DCC_OK) {
        status = dcc_rest_edit_message_flags(client, params->channel_id, params->message_id, body.data, cb, user_data);
    }
    dcc_rest_buffer_deinit(&body);
    return status;
}
