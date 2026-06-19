#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_resolved_object_map(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_gateway_resolved_entry_parse_fn parse_entry,
    dcc_json_gateway_resolved_map_finish_fn finish_map
) {
    if (parser == NULL || out == NULL || parse_entry == NULL || finish_map == NULL) {
        return DCC_ERR_JSON;
    }
    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end) {
        return DCC_ERR_JSON;
    }
    if (*parser->cur != '{') {
        return dcc_json_skip_value(parser);
    }

    parser->cur++;
    dcc_json_skip_ws(parser);
    size_t stored = 0;
    uint8_t truncated = 0;
    if (parser->cur < parser->end && *parser->cur == '}') {
        parser->cur++;
        finish_map(out, stored, truncated);
        return DCC_OK;
    }

    for (;;) {
        dcc_status_t status = dcc_json_gateway_parse_resolved_object_map_item(
            parser,
            out,
            parse_entry,
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
        if (*parser->cur == '}') {
            parser->cur++;
            finish_map(out, stored, truncated);
            return DCC_OK;
        }
        if (*parser->cur != ',') {
            return DCC_ERR_JSON;
        }
        parser->cur++;
        dcc_json_skip_ws(parser);
    }
}
