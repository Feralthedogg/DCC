#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_guild_channel_array(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out
) {
    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end || *parser->cur != '[' || out == NULL) {
        return DCC_ERR_JSON;
    }

    parser->cur++;
    dcc_json_skip_ws(parser);
    size_t stored = 0;
    uint8_t truncated = 0;
    if (parser->cur < parser->end && *parser->cur == ']') {
        parser->cur++;
        goto done;
    }

    for (;;) {
        dcc_status_t status = dcc_json_gateway_parse_guild_channel_array_item(
            parser,
            out,
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
            goto done;
        }
        if (*parser->cur != ',') {
            return DCC_ERR_JSON;
        }
        parser->cur++;
        dcc_json_skip_ws(parser);
    }

done:
    dcc_json_gateway_finish_guild_channel_array(out, stored, truncated);
    return DCC_OK;
}
