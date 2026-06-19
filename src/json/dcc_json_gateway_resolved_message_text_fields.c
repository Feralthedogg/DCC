#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_resolved_message_text_field(
    dcc_json_parser_t *parser,
    dcc_message_t *message,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_resolved_message_buffers_t *buffers,
    int *handled
) {
    if (parser == NULL || message == NULL || buffers == NULL || handled == NULL) {
        return DCC_ERR_JSON;
    }

    *handled = 1;
    char *target = NULL;
    size_t target_cap = 0;
    const char **field = NULL;

    if (key_id == DCC_JSON_KEY_CONTENT) {
        target = buffers->content;
        target_cap = buffers->content_cap;
        field = &message->content;
    } else if (key_id == DCC_JSON_KEY_TIMESTAMP) {
        target = buffers->timestamp;
        target_cap = buffers->timestamp_cap;
        field = &message->timestamp;
    } else if (key_id == DCC_JSON_KEY_EDITED_TIMESTAMP) {
        target = buffers->edited_timestamp;
        target_cap = buffers->edited_timestamp_cap;
        field = &message->edited_timestamp;
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
    return DCC_OK;
}
