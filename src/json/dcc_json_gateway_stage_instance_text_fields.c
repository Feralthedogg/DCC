#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_stage_instance_text_field(
    dcc_json_parser_t *parser,
    dcc_stage_instance_t *stage_instance,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_stage_instance_buffers_t *buffers,
    int *handled
) {
    if (parser == NULL || stage_instance == NULL || buffers == NULL || handled == NULL) {
        return DCC_ERR_JSON;
    }

    if (key_id != DCC_JSON_KEY_TOPIC) {
        *handled = 0;
        return DCC_OK;
    }

    int has_value = 0;
    dcc_status_t status = dcc_json_parse_nullable_string_buffer(
        parser,
        buffers->topic,
        buffers->topic_cap,
        &has_value
    );
    if (status != DCC_OK) {
        return status;
    }
    stage_instance->topic = has_value ? buffers->topic : NULL;
    *handled = 1;
    return DCC_OK;
}
