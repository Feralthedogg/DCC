#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_scheduled_event_numeric_scalar_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    switch (key_id) {
        case DCC_JSON_KEY_USER_COUNT:
        case DCC_JSON_KEY_PRIVACY_LEVEL:
        case DCC_JSON_KEY_ENTITY_TYPE:
            break;
        default:
            *handled = 0;
            return DCC_OK;
    }

    if (key_id == DCC_JSON_KEY_USER_COUNT) {
        uint32_t value = 0;
        dcc_status_t status = dcc_json_parse_u32_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        out->scheduled_event.user_count = value;
        out->has_scheduled_event = 1;
    } else if (key_id == DCC_JSON_KEY_PRIVACY_LEVEL) {
        uint32_t value = 0;
        dcc_status_t status = dcc_json_parse_u32_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.privacy_level = value;
        out->scheduled_event.privacy_level = value;
        out->stage_instance.privacy_level = value;
        out->has_scheduled_event = 1;
        out->has_stage_instance = 1;
    } else if (key_id == DCC_JSON_KEY_ENTITY_TYPE) {
        uint32_t value = 0;
        dcc_status_t status = dcc_json_parse_u32_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.entity_type = value;
        out->scheduled_event.entity_type = value;
        out->has_scheduled_event = 1;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    *handled = 1;
    return DCC_OK;
}
