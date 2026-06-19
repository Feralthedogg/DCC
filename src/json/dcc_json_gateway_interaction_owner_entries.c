#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_authorizing_integration_owner_entry(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    size_t *stored,
    uint8_t *truncated
) {
    if (parser == NULL || out == NULL || stored == NULL || truncated == NULL) {
        return DCC_ERR_JSON;
    }

    dcc_snowflake_t owner_type = 0;
    dcc_status_t status = dcc_json_parse_snowflake_map_key(parser, &owner_type);
    if (status != DCC_OK) {
        return status;
    }
    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end || *parser->cur != ':') {
        return DCC_ERR_JSON;
    }
    parser->cur++;
    dcc_json_skip_ws(parser);

    uint64_t owner_id = 0;
    int has_value = 0;
    status = dcc_json_parse_nullable_u64_token(parser, &owner_id, &has_value);
    if (status != DCC_OK) {
        return status;
    }
    if (has_value && owner_type <= UINT32_MAX) {
        if (*stored < DCC_JSON_GATEWAY_INTERACTION_AUTH_OWNER_CAP) {
            out->interaction_authorizing_integration_owners[*stored].type = (uint32_t)owner_type;
            out->interaction_authorizing_integration_owners[*stored].owner_id = owner_id;
            (*stored)++;
        } else {
            *truncated = 1;
        }
    }

    return DCC_OK;
}
