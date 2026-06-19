#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_message_builders_internal.h"

dcc_status_t dcc_rest_edit_message_builder(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[112];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/channels/%llu/messages/%llu",
        (unsigned long long)channel_id,
        (unsigned long long)message_id
    );
    return status == DCC_OK
        ? dcc_rest_request_message_builder_json(client, DCC_REST_PATCH, path, message, cb, user_data)
        : status;
}
