#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_interaction_option_array(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    const dcc_interaction_option_t **items_out,
    size_t *count_out,
    uint8_t *truncated_out,
    unsigned depth
) {
    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end || out == NULL || items_out == NULL ||
        count_out == NULL || truncated_out == NULL) {
        return DCC_ERR_JSON;
    }

    *items_out = NULL;
    *count_out = 0;
    *truncated_out = 0;

    if (*parser->cur != '[') {
        return dcc_json_skip_value(parser);
    }

    size_t list_index = 0;
    dcc_interaction_option_t *items = NULL;
    dcc_status_t status = dcc_json_gateway_begin_interaction_option_array(
        parser,
        out,
        items_out,
        truncated_out,
        depth,
        &list_index,
        &items
    );
    if (status != DCC_OK || items == NULL) {
        return status;
    }

    parser->cur++;
    return dcc_json_gateway_parse_interaction_option_array_loop(
        parser,
        out,
        items,
        list_index,
        depth,
        count_out,
        truncated_out
    );
}
