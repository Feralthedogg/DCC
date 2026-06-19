#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_resolved_member_map_item(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    size_t *stored,
    uint8_t *truncated,
    uint8_t *permissions_truncated
) {
    dcc_snowflake_t key = 0;
    dcc_status_t status = DCC_OK;

    if (parser == NULL || out == NULL || stored == NULL ||
        truncated == NULL || permissions_truncated == NULL) {
        return DCC_ERR_JSON;
    }

    status = dcc_json_parse_snowflake_map_key(parser, &key);
    if (status != DCC_OK) {
        return status;
    }
    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end || *parser->cur != ':') {
        return DCC_ERR_JSON;
    }
    parser->cur++;
    dcc_json_skip_ws(parser);

    if (parser->cur < parser->end && *parser->cur == '{' &&
        *stored < DCC_JSON_GATEWAY_INTERACTION_RESOLVED_CAP) {
        status = dcc_json_gateway_parse_resolved_member_entry(parser, out, key, *stored);
        if (status != DCC_OK) {
            return status;
        }
        (*stored)++;
        return DCC_OK;
    }

    status = dcc_json_skip_value(parser);
    if (status != DCC_OK) {
        return status;
    }
    *truncated = 1;
    *permissions_truncated = 1;
    return DCC_OK;
}
