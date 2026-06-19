#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_thread_list_text_field(
    dcc_json_parser_t *parser,
    dcc_channel_t *thread,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_thread_buffers_t *buffers,
    int *handled
) {
    int has_value = 0;

    if (handled == NULL || buffers == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id == DCC_JSON_KEY_NAME) {
        dcc_status_t status = dcc_json_parse_nullable_string_buffer(
            parser,
            buffers->name,
            buffers->name_cap,
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        thread->name = has_value ? buffers->name : NULL;
    } else if (key_id == DCC_JSON_KEY_TOPIC) {
        dcc_status_t status = dcc_json_parse_nullable_string_buffer(
            parser,
            buffers->topic,
            buffers->topic_cap,
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        thread->topic = has_value ? buffers->topic : NULL;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    *handled = 1;
    return DCC_OK;
}
