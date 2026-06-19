#include "internal/rest/dcc_rest_request_internal.h"

dcc_status_t dcc_rest_create_guild_sticker_params(
    dcc_client_t *client,
    const dcc_guild_sticker_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (params == NULL ||
        params->size < sizeof(*params) ||
        params->guild_id == 0 ||
        params->name == NULL ||
        params->name[0] == '\0' ||
        params->description == NULL ||
        params->tags == NULL ||
        params->tags[0] == '\0' ||
        params->filename == NULL ||
        params->filename[0] == '\0' ||
        params->data == NULL ||
        params->data_len == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    const dcc_rest_multipart_field_t fields[] = {
        { .name = "name", .value = params->name },
        { .name = "description", .value = params->description },
        { .name = "tags", .value = params->tags },
    };
    const dcc_rest_multipart_file_t file = {
        .field_name = "file",
        .filename = params->filename,
        .content_type = params->content_type,
        .data = params->data,
        .data_len = params->data_len,
    };
    return dcc_rest_create_guild_sticker_multipart(
        client,
        params->guild_id,
        fields,
        sizeof(fields) / sizeof(fields[0]),
        &file,
        cb,
        user_data
    );
}
