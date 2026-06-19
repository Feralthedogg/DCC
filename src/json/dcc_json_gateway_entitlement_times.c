#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_entitlement_time_field(
    dcc_json_parser_t *parser,
    dcc_entitlement_t *entitlement,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_entitlement_buffers_t *buffers,
    int *handled
) {
    int has_value = 0;

    if (handled == NULL || buffers == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id == DCC_JSON_KEY_SCHEDULED_START_TIME ||
        key_id == DCC_JSON_KEY_STARTS_AT) {
        dcc_status_t status = dcc_json_parse_nullable_string_buffer(
            parser,
            buffers->start_time,
            buffers->start_time_cap,
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        entitlement->start_time = has_value ? buffers->start_time : NULL;
    } else if (key_id == DCC_JSON_KEY_SCHEDULED_END_TIME ||
               key_id == DCC_JSON_KEY_ENDS_AT) {
        dcc_status_t status = dcc_json_parse_nullable_string_buffer(
            parser,
            buffers->end_time,
            buffers->end_time_cap,
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        entitlement->end_time = has_value ? buffers->end_time : NULL;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    *handled = 1;
    return DCC_OK;
}
