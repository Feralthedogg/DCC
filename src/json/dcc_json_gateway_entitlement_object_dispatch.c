#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_entitlement_object_field(
    dcc_json_parser_t *parser,
    dcc_entitlement_t *entitlement,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_entitlement_buffers_t *buffers
) {
    int handled = 0;
    dcc_status_t status = dcc_json_gateway_parse_entitlement_id_field(
        parser,
        entitlement,
        key_id,
        &handled
    );
    if (status != DCC_OK) {
        return status;
    }
    if (!handled) {
        status = dcc_json_gateway_parse_entitlement_scalar_field(
            parser,
            entitlement,
            key_id,
            &handled
        );
        if (status != DCC_OK) {
            return status;
        }
    }
    if (!handled) {
        status = dcc_json_gateway_parse_entitlement_time_field(
            parser,
            entitlement,
            key_id,
            buffers,
            &handled
        );
        if (status != DCC_OK) {
            return status;
        }
    }
    if (!handled) {
        status = dcc_json_skip_value(parser);
        if (status != DCC_OK) {
            return status;
        }
    }
    return DCC_OK;
}
