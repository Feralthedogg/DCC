#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_interaction_data_text_field(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (parser == NULL || out == NULL || handled == NULL) {
        return DCC_ERR_JSON;
    }

    if (key_id == DCC_JSON_KEY_NAME) {
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_string_buffer(
            parser,
            out->interaction_name,
            sizeof(out->interaction_name),
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        out->interaction.name = has_value ? out->interaction_name : NULL;
        out->gateway.name = has_value ? out->interaction_name : NULL;
        out->has_interaction = 1;
        *handled = 1;
        return DCC_OK;
    }

    if (key_id == DCC_JSON_KEY_CUSTOM_ID) {
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_string_buffer(
            parser,
            out->interaction_custom_id,
            sizeof(out->interaction_custom_id),
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        out->interaction.custom_id = has_value ? out->interaction_custom_id : NULL;
        out->gateway.code = has_value ? out->interaction_custom_id : NULL;
        out->has_interaction = 1;
        *handled = 1;
        return DCC_OK;
    }

    *handled = 0;
    return DCC_OK;
}
