#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_core_interaction_object_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = DCC_OK;
    if (key_id == DCC_JSON_KEY_DATA) {
        status = dcc_json_gateway_parse_interaction_data(parser, out);
        if (status == DCC_OK) {
            out->has_interaction = 1;
        }
    } else if (key_id == DCC_JSON_KEY_AUTHORIZING_INTEGRATION_OWNERS) {
        status = dcc_json_gateway_parse_authorizing_integration_owners(parser, out);
    } else if (key_id == DCC_JSON_KEY_ENTITLEMENTS) {
        status = dcc_json_gateway_parse_interaction_entitlements(parser, out);
    } else if (key_id == DCC_JSON_KEY_USER || key_id == DCC_JSON_KEY_INVITER) {
        status = dcc_json_gateway_parse_user(parser, out);
    } else if (key_id == DCC_JSON_KEY_MEMBER) {
        status = dcc_json_gateway_parse_member(parser, out);
    } else if (key_id == DCC_JSON_KEY_ROLE) {
        status = dcc_json_gateway_parse_role(parser, out);
    } else if (key_id == DCC_JSON_KEY_ACTION) {
        status = dcc_json_gateway_parse_action(parser, out);
    } else if (key_id == DCC_JSON_KEY_CLIENT_STATUS) {
        status = dcc_json_gateway_parse_client_status(parser, out);
    } else {
        *handled = 0;
        return DCC_OK;
    }

    if (status != DCC_OK) {
        return status;
    }
    *handled = 1;
    return DCC_OK;
}
