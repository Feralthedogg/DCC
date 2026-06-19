#include "internal/json/dcc_json_internal.h"

#include <stddef.h>

dcc_status_t dcc_json_parse_gateway_payload(const char *input, size_t len, dcc_json_gateway_payload_t *out) {
    if (input == NULL || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_json_gateway_payload_reset(out);

    /* Gateway frames are parsed on the hot path; the direct parser validates
       strings and UTF-8 while walking instead of paying an unused stage1 pass. */
    dcc_json_parser_t parser;
    parser.cur = input;
    parser.end = input + len;
    parser.doc = NULL;

    dcc_json_skip_ws(&parser);
    if (parser.cur >= parser.end || *parser.cur != '{') {
        return DCC_ERR_JSON;
    }
    parser.cur++;
    dcc_json_skip_ws(&parser);
    if (parser.cur < parser.end && *parser.cur == '}') {
        parser.cur++;
    } else {
        for (;;) {
            dcc_json_key_id_t key_id = DCC_JSON_KEY_UNKNOWN;
            dcc_status_t status = DCC_OK;

            if (dcc_json_parse_gateway_key(&parser, &key_id) != 0) {
                return DCC_ERR_JSON;
            }
            dcc_json_skip_ws(&parser);
            if (parser.cur >= parser.end || *parser.cur != ':') {
                return DCC_ERR_JSON;
            }
            parser.cur++;
            dcc_json_skip_ws(&parser);

            status = dcc_json_gateway_parse_payload_value(&parser, key_id, out);
            if (status != DCC_OK) {
                return status;
            }

            dcc_json_skip_ws(&parser);
            if (parser.cur >= parser.end) {
                return DCC_ERR_JSON;
            }
            if (*parser.cur == '}') {
                parser.cur++;
                break;
            }
            if (*parser.cur != ',') {
                return DCC_ERR_JSON;
            }
            parser.cur++;
            dcc_json_skip_ws(&parser);
        }
    }

    dcc_json_skip_ws(&parser);
    if (parser.cur != parser.end) {
        return DCC_ERR_JSON;
    }
    dcc_json_gateway_normalize_payload(out);
    return DCC_OK;
}
