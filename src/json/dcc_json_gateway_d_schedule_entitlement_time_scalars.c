#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_schedule_entitlement_time_scalar_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    int has_value = 0;

    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id == DCC_JSON_KEY_SCHEDULED_START_TIME || key_id == DCC_JSON_KEY_STARTS_AT) {
        dcc_status_t status = dcc_json_parse_nullable_string_buffer(
            parser,
            out->gateway_start_time,
            sizeof(out->gateway_start_time),
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.start_time = has_value ? out->gateway_start_time : NULL;
        out->scheduled_event.start_time = has_value ? out->gateway_start_time : NULL;
        out->entitlement.start_time = has_value ? out->gateway_start_time : NULL;
        out->has_scheduled_event = 1;
        out->has_entitlement = 1;
    }
    else if (key_id == DCC_JSON_KEY_SCHEDULED_END_TIME || key_id == DCC_JSON_KEY_ENDS_AT) {
        dcc_status_t status = dcc_json_parse_nullable_string_buffer(
            parser,
            out->gateway_end_time,
            sizeof(out->gateway_end_time),
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.end_time = has_value ? out->gateway_end_time : NULL;
        out->scheduled_event.end_time = has_value ? out->gateway_end_time : NULL;
        out->entitlement.end_time = has_value ? out->gateway_end_time : NULL;
        out->has_scheduled_event = 1;
        out->has_entitlement = 1;
    }
    else {
        *handled = 0;
        return DCC_OK;
    }

    *handled = 1;
    return DCC_OK;
}
