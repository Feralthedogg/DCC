#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_sticker_list_text_field(
    dcc_json_parser_t *parser,
    dcc_sticker_t *sticker,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_sticker_buffers_t *buffers,
    int *handled
) {
    if (handled == NULL || buffers == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    char *target = NULL;
    size_t target_cap = 0;
    const char **field = NULL;
    if (key_id == DCC_JSON_KEY_NAME) {
        target = buffers->name;
        target_cap = buffers->name_cap;
        field = &sticker->name;
    } else if (key_id == DCC_JSON_KEY_DESCRIPTION) {
        target = buffers->description;
        target_cap = buffers->description_cap;
        field = &sticker->description;
    } else if (key_id == DCC_JSON_KEY_TAGS) {
        target = buffers->tags;
        target_cap = buffers->tags_cap;
        field = &sticker->tags;
    } else if (key_id == DCC_JSON_KEY_ASSET) {
        target = buffers->asset;
        target_cap = buffers->asset_cap;
        field = &sticker->asset;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    int has_value = 0;
    dcc_status_t status = dcc_json_parse_nullable_string_buffer(
        parser,
        target,
        target_cap,
        &has_value
    );
    if (status != DCC_OK) {
        return status;
    }
    *field = has_value ? target : NULL;
    *handled = 1;
    return DCC_OK;
}
