#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_stage_instance_scalar_field(
    dcc_json_parser_t *parser,
    dcc_stage_instance_t *stage_instance,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (parser == NULL || stage_instance == NULL || handled == NULL) {
        return DCC_ERR_JSON;
    }

    if (key_id == DCC_JSON_KEY_PRIVACY_LEVEL) {
        uint32_t value = 0;
        dcc_status_t status = dcc_json_parse_u32_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        stage_instance->privacy_level = value;
        *handled = 1;
        return DCC_OK;
    }

    if (key_id == DCC_JSON_KEY_DISCOVERABLE_DISABLED) {
        int value = 0;
        dcc_status_t status = dcc_json_parse_bool_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        stage_instance->discoverable_disabled = value ? 1U : 0U;
        *handled = 1;
        return DCC_OK;
    }

    *handled = 0;
    return DCC_OK;
}
