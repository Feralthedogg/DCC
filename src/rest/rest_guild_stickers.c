#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

dcc_status_t dcc_rest_get_guild_stickers(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[88];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/guilds/%llu/stickers", (unsigned long long)guild_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_GET, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_get_guild_sticker(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t sticker_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[120];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/guilds/%llu/stickers/%llu",
        (unsigned long long)guild_id,
        (unsigned long long)sticker_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_GET, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_create_guild_sticker_multipart(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const dcc_rest_multipart_field_t *fields,
    size_t field_count,
    const dcc_rest_multipart_file_t *file,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[88];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/guilds/%llu/stickers", (unsigned long long)guild_id);
    return status == DCC_OK
        ? dcc_rest_request_method_multipart(client, DCC_REST_POST, path, fields, field_count, file, 1, cb, user_data)
        : status;
}

dcc_status_t dcc_rest_modify_guild_sticker(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t sticker_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[120];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/guilds/%llu/stickers/%llu",
        (unsigned long long)guild_id,
        (unsigned long long)sticker_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_PATCH, path, json_body, cb, user_data) : status;
}

dcc_status_t dcc_rest_delete_guild_sticker(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t sticker_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[120];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/guilds/%llu/stickers/%llu",
        (unsigned long long)guild_id,
        (unsigned long long)sticker_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_DELETE, path, NULL, cb, user_data) : status;
}
