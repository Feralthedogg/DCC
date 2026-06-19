#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_role_object_field(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id
) {
    if (parser == NULL || out == NULL) {
        return DCC_ERR_JSON;
    }

    int handled = 0;
    dcc_status_t status = dcc_json_gateway_parse_role_field(
        parser,
        &out->role,
        key_id,
        out->gateway_role_name,
        sizeof(out->gateway_role_name),
        &out->cache_fields,
        &handled
    );
    if (status != DCC_OK) {
        return status;
    }
    if (!handled) {
        return dcc_json_skip_value(parser);
    }

    if (key_id == DCC_JSON_KEY_ID) {
        out->gateway.role_id = out->role.id;
    } else if (key_id == DCC_JSON_KEY_NAME) {
        out->gateway.name = out->role.name;
    }
    return DCC_OK;
}
