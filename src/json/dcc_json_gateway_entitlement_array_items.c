#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_interaction_entitlement_array_item(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    size_t *stored,
    uint8_t *truncated
) {
    if (parser->cur < parser->end && *parser->cur == '{' &&
        *stored < DCC_JSON_GATEWAY_INTERACTION_ENTITLEMENT_CAP) {
        dcc_status_t status = dcc_json_gateway_parse_entitlement_object(
            parser,
            &out->interaction_entitlements[*stored],
            out->interaction_entitlement_start_times[*stored],
            sizeof(out->interaction_entitlement_start_times[*stored]),
            out->interaction_entitlement_end_times[*stored],
            sizeof(out->interaction_entitlement_end_times[*stored])
        );
        if (status != DCC_OK) {
            return status;
        }
        (*stored)++;
        return DCC_OK;
    }

    if (parser->cur < parser->end && *parser->cur == '{') {
        *truncated = 1;
    }
    return dcc_json_skip_value(parser);
}
