#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_attachment_field(
    dcc_json_parser_t *parser,
    dcc_attachment_t *attachment,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_attachment_buffers_t *buffers
) {
    if (parser == NULL || attachment == NULL || buffers == NULL) {
        return DCC_ERR_JSON;
    }

    int handled = 0;
    dcc_status_t status = dcc_json_gateway_parse_attachment_id_field(
        parser,
        attachment,
        key_id,
        &handled
    );
    if (status != DCC_OK) {
        return status;
    }
    if (!handled) {
        status = dcc_json_gateway_parse_attachment_numeric_field(
            parser,
            attachment,
            key_id,
            &handled
        );
        if (status != DCC_OK) {
            return status;
        }
    }
    if (!handled) {
        status = dcc_json_gateway_parse_attachment_string_field(
            parser,
            attachment,
            key_id,
            buffers,
            &handled
        );
        if (status != DCC_OK) {
            return status;
        }
    }
    if (!handled) {
        status = dcc_json_gateway_parse_attachment_media_field(
            parser,
            attachment,
            key_id,
            &handled
        );
        if (status != DCC_OK) {
            return status;
        }
    }
    if (!handled) {
        status = dcc_json_skip_value(parser);
    }
    return status;
}
