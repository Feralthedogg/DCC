#include "internal/json/dcc_json_internal.h"

#include <stddef.h>

dcc_status_t dcc_json_parse_entity_metadata_location(
    dcc_json_parser_t *parser,
    char *location,
    size_t location_cap,
    const char **out_location
) {
    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end || location == NULL || out_location == NULL) {
        return DCC_ERR_JSON;
    }
    *out_location = NULL;
    if (*parser->cur == 'n') {
        return dcc_json_match_literal(parser, "null") == 0 ? DCC_OK : DCC_ERR_JSON;
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

        dcc_status_t status = dcc_json_parse_entity_metadata_location_field(
            parser,
            key_id,
            location,
            location_cap,
            out_location
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
            return DCC_OK;
        }
        if (*parser->cur != ',') {
            return DCC_ERR_JSON;
        }
        parser->cur++;
        dcc_json_skip_ws(parser);
    }
}
