#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_resolved_message_author_field(
    dcc_json_parser_t *parser,
    dcc_message_t *message,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_resolved_message_buffers_t *buffers,
    int *handled
) {
    if (parser == NULL || message == NULL || buffers == NULL || handled == NULL) {
        return DCC_ERR_JSON;
    }

    if (key_id != DCC_JSON_KEY_AUTHOR) {
        *handled = 0;
        return DCC_OK;
    }

    dcc_status_t status = dcc_json_gateway_parse_user_object(
        parser,
        &message->author,
        buffers->author_username,
        buffers->author_username_cap,
        buffers->author_global_name,
        buffers->author_global_name_cap,
        buffers->author_avatar,
        buffers->author_avatar_cap
    );
    if (status != DCC_OK) {
        return status;
    }

    *handled = 1;
    return DCC_OK;
}
