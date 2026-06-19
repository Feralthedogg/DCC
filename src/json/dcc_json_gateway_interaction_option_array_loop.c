#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_interaction_option_array_loop(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_interaction_option_t *items,
    size_t list_index,
    unsigned depth,
    size_t *count_out,
    uint8_t *truncated_out
) {
    if (parser == NULL || out == NULL || items == NULL ||
        count_out == NULL || truncated_out == NULL) {
        return DCC_ERR_JSON;
    }

    dcc_json_skip_ws(parser);
    if (parser->cur < parser->end && *parser->cur == ']') {
        parser->cur++;
        return DCC_OK;
    }

    size_t index = 0;
    for (;;) {
        dcc_status_t status = dcc_json_gateway_parse_interaction_option_array_item(
            parser,
            out,
            items,
            list_index,
            index,
            depth,
            count_out,
            truncated_out
        );
        if (status != DCC_OK) {
            return status;
        }
        index++;

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
        dcc_json_skip_ws(parser);
    }
}
