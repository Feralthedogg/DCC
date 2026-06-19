#include "internal/rest/dcc_rest_buffer_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_query_append_internal.h"
#include "internal/rest/dcc_rest_request_core_internal.h"

dcc_status_t dcc_rest_get_global_commands(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[80];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/applications/%llu/commands", (unsigned long long)application_id);
    return status == DCC_OK ? dcc_rest_request_with_query(client, DCC_REST_GET, path, query, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_get_global_commands_with_localizations(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    uint8_t with_localizations,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_rest_buffer_t query = {0};
    dcc_status_t status = dcc_rest_query_append_bool(&query, "with_localizations", with_localizations);
    if (status == DCC_OK) {
        status = dcc_rest_get_global_commands(client, application_id, query.data, cb, user_data);
    }
    dcc_rest_buffer_deinit(&query);
    return status;
}

dcc_status_t dcc_rest_get_global_command(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t command_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[112];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/applications/%llu/commands/%llu",
        (unsigned long long)application_id,
        (unsigned long long)command_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_GET, path, NULL, cb, user_data) : status;
}
