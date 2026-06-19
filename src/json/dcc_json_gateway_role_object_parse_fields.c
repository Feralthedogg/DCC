#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_role_list_object_field(
    dcc_json_parser_t *parser,
    dcc_role_t *role,
    char *name,
    size_t name_cap
) {
    dcc_json_key_id_t key_id = DCC_JSON_KEY_UNKNOWN;
    if (dcc_json_parse_gateway_key(parser, &key_id) != 0) {
        return DCC_ERR_JSON;
    }
    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end || *parser->cur != ':') {
        return DCC_ERR_JSON;
    }
    parser->cur++;
    dcc_json_skip_ws(parser);

    int handled = 0;
    dcc_status_t status = dcc_json_gateway_parse_role_field(
        parser,
        role,
        key_id,
        name,
        name_cap,
        NULL,
        &handled
    );
    if (status != DCC_OK) {
        return status;
    }
    if (!handled) {
        status = dcc_json_skip_value(parser);
    }
    return status;
}
