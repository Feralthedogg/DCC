#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_presence_client_status_fields(
    dcc_json_parser_t *parser,
    dcc_presence_t *presence,
    const dcc_json_gateway_presence_status_buffers_t *buffers
) {
    if (parser == NULL || presence == NULL || buffers == NULL) {
        return DCC_ERR_JSON;
    }

    for (;;) {
        dcc_json_key_id_t key_id = DCC_JSON_KEY_UNKNOWN;
        int handled = 0;
        dcc_status_t status = DCC_OK;

        if (dcc_json_parse_gateway_key(parser, &key_id) != 0) {
            return DCC_ERR_JSON;
        }
        dcc_json_skip_ws(parser);
        if (parser->cur >= parser->end || *parser->cur != ':') {
            return DCC_ERR_JSON;
        }
        parser->cur++;
        dcc_json_skip_ws(parser);

        status = dcc_json_gateway_parse_presence_client_status_field(
            parser,
            presence,
            key_id,
            buffers->desktop_status,
            buffers->desktop_status_cap,
            buffers->mobile_status,
            buffers->mobile_status_cap,
            buffers->web_status,
            buffers->web_status_cap,
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
