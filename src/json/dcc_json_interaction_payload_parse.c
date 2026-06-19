#include "internal/json/dcc_json_internal.h"

#include <stddef.h>

dcc_status_t dcc_json_parse_interaction_payload(const char *input, size_t len, dcc_json_gateway_payload_t *out) {
    if (input == NULL || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_json_gateway_payload_reset(out);

    dcc_json_parser_t parser;
    parser.cur = input;
    parser.end = input + len;
    parser.doc = NULL;

    dcc_status_t status = dcc_json_gateway_parse_d(&parser, out);
    if (status != DCC_OK) {
        return status;
    }
    dcc_json_skip_ws(&parser);
    if (parser.cur != parser.end) {
        return DCC_ERR_JSON;
    }
    dcc_json_gateway_normalize_payload(out);
    return DCC_OK;
}
