#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_interaction_option_meta_field(
    dcc_json_parser_t *parser,
    dcc_interaction_option_t *option,
    dcc_json_key_id_t key_id,
    dcc_json_gateway_interaction_option_state_t *state,
    int *handled
) {
    if (parser == NULL || option == NULL || state == NULL || handled == NULL) {
        return DCC_ERR_JSON;
    }

    if (key_id == DCC_JSON_KEY_NAME) {
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_string_buffer(
            parser,
            state->name,
            DCC_JSON_GATEWAY_INTERACTION_OPTION_NAME_CAP,
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        option->name = has_value ? state->name : NULL;
        *handled = 1;
        return DCC_OK;
    }

    if (key_id == DCC_JSON_KEY_TYPE) {
        uint32_t value = 0;
        dcc_status_t status = dcc_json_parse_u32_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        option->type = value;
        *handled = 1;
        return DCC_OK;
    }

    if (key_id == DCC_JSON_KEY_FOCUSED) {
        int value = 0;
        dcc_status_t status = dcc_json_parse_bool_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        option->focused = value ? 1U : 0U;
        *handled = 1;
        return DCC_OK;
    }

    *handled = 0;
    return DCC_OK;
}
