#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_skip_array(dcc_json_parser_t *parser) {
    parser->cur++;
    dcc_json_skip_ws(parser);
    if (parser->cur < parser->end && *parser->cur == ']') {
        parser->cur++;
        return DCC_OK;
    }
    for (;;) {
        dcc_status_t status = dcc_json_skip_value(parser);
        if (status != DCC_OK) {
            return status;
        }
        dcc_json_skip_ws(parser);
        if (parser->cur >= parser->end) {
            return DCC_ERR_JSON;
        }
        if (*parser->cur == ']') {
            parser->cur++;
            return DCC_OK;
        }
        if (*parser->cur != ',') {
            return DCC_ERR_JSON;
        }
        parser->cur++;
    }
}
