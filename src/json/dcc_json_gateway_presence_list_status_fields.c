#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_presence_list_status_field(
    dcc_json_parser_t *parser,
    dcc_presence_t *presence,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_presence_list_buffers_t *buffers,
    int *handled
) {
    if (parser == NULL || presence == NULL || buffers == NULL || handled == NULL) {
        return DCC_ERR_JSON;
    }

    if (key_id == DCC_JSON_KEY_STATUS) {
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_string_buffer(
            parser,
            buffers->status_text,
            buffers->status_text_cap,
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        presence->status = has_value ? buffers->status_text : NULL;
        *handled = 1;
        return DCC_OK;
    }

    if (key_id == DCC_JSON_KEY_SINCE) {
        uint64_t value = 0;
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
        if (status != DCC_OK) {
            return status;
        }
        presence->since = has_value ? value : 0;
        *handled = 1;
        return DCC_OK;
    }

    if (key_id == DCC_JSON_KEY_CLIENT_STATUS) {
        *handled = 1;
        return dcc_json_gateway_parse_presence_client_status(
            parser,
            presence,
            buffers->desktop_status,
            buffers->desktop_status_cap,
            buffers->mobile_status,
            buffers->mobile_status_cap,
            buffers->web_status,
            buffers->web_status_cap
        );
    }

    *handled = 0;
    return DCC_OK;
}
