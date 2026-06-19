#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d(dcc_json_parser_t *parser, dcc_json_gateway_payload_t *out) {
    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end) {
        return DCC_ERR_JSON;
    }

    if (*parser->cur == 't' || *parser->cur == 'f') {
        int value = 0;
        dcc_status_t status = dcc_json_parse_bool_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        out->has_invalid_session_resumable = 1;
        out->invalid_session_resumable = value;
        return DCC_OK;
    }

    if (*parser->cur != '{') {
        return dcc_json_skip_value(parser);
    }

    parser->cur++;
    dcc_json_skip_ws(parser);
    if (parser->cur < parser->end && *parser->cur == '}') {
        parser->cur++;
        return DCC_OK;
    }

    for (;;) {
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
        dcc_status_t status = dcc_json_gateway_parse_d_core_key(parser, out, key_id, &handled);
        if (status != DCC_OK) {
            return status;
        }
        if (!handled) {
            status = dcc_json_gateway_parse_d_id_key(parser, out, key_id, &handled);
            if (status != DCC_OK) {
                return status;
            }
        }
        if (!handled) {
            status = dcc_json_gateway_parse_d_scalar_key(parser, out, key_id, &handled);
            if (status != DCC_OK) {
                return status;
            }
        }
        if (!handled) {
            status = dcc_json_gateway_parse_d_collection_key(parser, out, key_id, &handled);
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
