#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_attachment_media_field(
    dcc_json_parser_t *parser,
    dcc_attachment_t *attachment,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id == DCC_JSON_KEY_EPHEMERAL) {
        if (parser->cur < parser->end && *parser->cur == 'n') {
            if (dcc_json_match_literal(parser, "null") != 0) {
                return DCC_ERR_JSON;
            }
            attachment->ephemeral = 0U;
        } else {
            int value = 0;
            dcc_status_t status = dcc_json_parse_bool_direct(parser, &value);
            if (status != DCC_OK) {
                return status;
            }
            attachment->ephemeral = value ? 1U : 0U;
        }
    } else if (key_id == DCC_JSON_KEY_DURATION_SECS) {
        if (parser->cur < parser->end && *parser->cur == 'n') {
            if (dcc_json_match_literal(parser, "null") != 0) {
                return DCC_ERR_JSON;
            }
            attachment->duration_secs = 0.0;
        } else {
            double value = 0.0;
            dcc_status_t status = dcc_json_parse_double_direct(parser, &value);
            if (status != DCC_OK) {
                return status;
            }
            attachment->duration_secs = value;
        }
    } else {
        *handled = 0;
        return DCC_OK;
    }

    *handled = 1;
    return DCC_OK;
}
