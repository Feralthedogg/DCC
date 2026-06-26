#include "internal/rest/dcc_rest_buffer_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_core_internal.h"

#include <stdio.h>
#include <stdlib.h>

dcc_status_t dcc_rest_pin_message(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[120];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/channels/%llu/messages/pins/%llu",
        (unsigned long long)channel_id,
        (unsigned long long)message_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_PUT, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_unpin_message(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[120];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/channels/%llu/messages/pins/%llu",
        (unsigned long long)channel_id,
        (unsigned long long)message_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_DELETE, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_get_channel_pins(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[88];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/channels/%llu/messages/pins", (unsigned long long)channel_id);
    return status == DCC_OK ? dcc_rest_request_with_query(client, DCC_REST_GET, path, query, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_get_channel_pins_page(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const char *before_iso8601,
    uint64_t limit,
    dcc_rest_cb cb,
    void *user_data
) {
    char limit_text[32];
    int written = snprintf(
        limit_text,
        sizeof(limit_text),
        "%llu",
        (unsigned long long)(limit != 0 ? limit : 50ULL)
    );
    if (written < 0 || (size_t)written >= sizeof(limit_text)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t query = {0};
    dcc_status_t status = dcc_rest_buffer_append_cstr(&query, "limit=");
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&query, limit_text);
    }

    char *encoded_before = NULL;
    if (status == DCC_OK && before_iso8601 != NULL && before_iso8601[0] != '\0') {
        status = dcc_rest_escape_path_segment(before_iso8601, &encoded_before);
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_cstr(&query, "&before=");
        }
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_cstr(&query, encoded_before);
        }
    }

    if (status == DCC_OK) {
        status = dcc_rest_get_channel_pins(client, channel_id, query.data, cb, user_data);
    }
    free(encoded_before);
    dcc_rest_buffer_deinit(&query);
    return status;
}

dcc_status_t dcc_rest_legacy_pin_message(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[96];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/channels/%llu/pins/%llu",
        (unsigned long long)channel_id,
        (unsigned long long)message_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_PUT, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_legacy_unpin_message(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[96];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/channels/%llu/pins/%llu",
        (unsigned long long)channel_id,
        (unsigned long long)message_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_DELETE, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_get_legacy_channel_pins(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[64];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/channels/%llu/pins", (unsigned long long)channel_id);
    return status == DCC_OK ? dcc_rest_request_with_query(client, DCC_REST_GET, path, query, NULL, cb, user_data) : status;
}
