#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_application_command_permission_object_fields(
    dcc_json_parser_t *parser,
    dcc_application_command_permission_t *permission
) {
    if (parser == NULL || permission == NULL) {
        return DCC_ERR_JSON;
    }

    for (;;) {
        dcc_json_key_id_t key_id = DCC_JSON_KEY_UNKNOWN;
        dcc_status_t status = DCC_OK;
        int handled = 0;

        if (dcc_json_parse_gateway_key(parser, &key_id) != 0) {
            return DCC_ERR_JSON;
        }
        dcc_json_skip_ws(parser);
        if (parser->cur >= parser->end || *parser->cur != ':') {
            return DCC_ERR_JSON;
        }
        parser->cur++;
        dcc_json_skip_ws(parser);

        status = dcc_json_gateway_parse_application_command_permission_field(
            parser,
            permission,
            key_id,
            &handled
        );
        if (status != DCC_OK) {
            return status;
        }
        if (!handled) {
            status = dcc_json_skip_value(parser);
            if (status != DCC_OK) {
                return status;
            }
        }

        dcc_json_skip_ws(parser);
        if (parser->cur >= parser->end) {
            return DCC_ERR_JSON;
        }
        if (*parser->cur == '}') {
            parser->cur++;
            return DCC_OK;
        }
        if (*parser->cur != ',') {
            return DCC_ERR_JSON;
        }
        parser->cur++;
        dcc_json_skip_ws(parser);
    }
}
