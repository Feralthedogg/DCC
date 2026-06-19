#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_activity_array_items(
    dcc_json_parser_t *parser,
    const dcc_json_gateway_activity_array_t *array
) {
    size_t stored = 0;
    uint8_t truncated = 0;

    if (parser == NULL || array == NULL) {
        return DCC_ERR_JSON;
    }

    dcc_json_skip_ws(parser);
    if (parser->cur < parser->end && *parser->cur == ']') {
        parser->cur++;
        dcc_json_gateway_finish_activity_array(array, 0, 0);
        return DCC_OK;
    }

    for (;;) {
        dcc_status_t status = dcc_json_gateway_parse_activity_array_item(
            parser,
            array,
            &stored,
            &truncated
        );
        if (status != DCC_OK) {
            return status;
        }

        dcc_json_skip_ws(parser);
        if (parser->cur >= parser->end) {
            return DCC_ERR_JSON;
        }
        if (*parser->cur == ']') {
            parser->cur++;
            dcc_json_gateway_finish_activity_array(array, stored, truncated);
            return DCC_OK;
        }
        if (*parser->cur != ',') {
            return DCC_ERR_JSON;
        }
        parser->cur++;
        dcc_json_skip_ws(parser);
    }
}
