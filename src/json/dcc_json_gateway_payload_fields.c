#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_payload_value(
    dcc_json_parser_t *parser,
    dcc_json_key_id_t key_id,
    dcc_json_gateway_payload_t *out
) {
    if (key_id == DCC_JSON_KEY_OP) {
        int64_t value = 0;
        dcc_status_t status = dcc_json_parse_i64_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        out->has_op = 1;
        out->op = value;
        return DCC_OK;
    }

    if (key_id == DCC_JSON_KEY_S) {
        dcc_json_skip_ws(parser);
        if (parser->cur < parser->end && *parser->cur == 'n') {
            return dcc_json_match_literal(parser, "null") == 0 ? DCC_OK : DCC_ERR_JSON;
        }
        uint64_t value = 0;
        dcc_status_t status = dcc_json_parse_u64_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        out->has_seq = 1;
        out->seq = value;
        return DCC_OK;
    }

    if (key_id == DCC_JSON_KEY_T) {
        dcc_json_skip_ws(parser);
        if (parser->cur < parser->end && *parser->cur == 'n') {
            return dcc_json_match_literal(parser, "null") == 0 ? DCC_OK : DCC_ERR_JSON;
        }
        if (dcc_json_parse_string_buffer(parser, out->event_name, sizeof(out->event_name)) != 0) {
            return DCC_ERR_JSON;
        }
        out->has_event_name = 1;
        return DCC_OK;
    }

    if (key_id == DCC_JSON_KEY_D) {
        return dcc_json_gateway_parse_d(parser, out);
    }

    return dcc_json_skip_value(parser);
}
