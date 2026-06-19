#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_interaction_data_id_field(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (parser == NULL || out == NULL || handled == NULL) {
        return DCC_ERR_JSON;
    }

    if (key_id == DCC_JSON_KEY_ID) {
        uint64_t value = 0;
        dcc_status_t status = dcc_json_parse_u64_token(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        out->interaction.command_id = value;
        out->gateway.target_id = value;
        out->has_interaction = 1;
        *handled = 1;
        return DCC_OK;
    }

    if (key_id == DCC_JSON_KEY_TARGET_ID) {
        uint64_t value = 0;
        dcc_status_t status = dcc_json_parse_u64_token(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        out->interaction.target_id = value;
        out->gateway.target_id = value;
        out->has_interaction = 1;
        *handled = 1;
        return DCC_OK;
    }

    *handled = 0;
    return DCC_OK;
}
