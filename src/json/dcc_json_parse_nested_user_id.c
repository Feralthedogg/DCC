#include "internal/json/dcc_json_internal.h"

#include <stdint.h>

dcc_status_t dcc_json_parse_nested_user_id(
    dcc_json_parser_t *parser,
    uint64_t *out,
    int *has_value
) {
    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end || out == NULL || has_value == NULL) {
        return DCC_ERR_JSON;
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

        if (key_id == DCC_JSON_KEY_ID) {
            uint64_t value = 0;
            int has_id = 0;
            dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_id);
            if (status != DCC_OK) {
                return status;
            }
            if (has_id) {
                *out = value;
                *has_value = 1;
            }
        } else {
            dcc_status_t status = dcc_json_skip_value(parser);
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
