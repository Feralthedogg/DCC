#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_emoji_list_text_field(
    dcc_json_parser_t *parser,
    dcc_emoji_t *emoji,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_emoji_buffers_t *buffers,
    int *handled
) {
    if (handled == NULL || buffers == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id != DCC_JSON_KEY_NAME) {
        *handled = 0;
        return DCC_OK;
    }

    int has_value = 0;
    dcc_status_t status = dcc_json_parse_nullable_string_buffer(
        parser,
        buffers->name,
        buffers->name_cap,
        &has_value
    );
    if (status != DCC_OK) {
        return status;
    }
    emoji->name = has_value ? buffers->name : NULL;
    *handled = 1;
    return DCC_OK;
}
