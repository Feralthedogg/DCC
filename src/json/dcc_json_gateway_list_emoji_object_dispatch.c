#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_emoji_list_object_field(
    dcc_json_parser_t *parser,
    dcc_emoji_t *emoji,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_emoji_buffers_t *buffers
) {
    int handled = 0;
    dcc_status_t status = dcc_json_gateway_parse_emoji_list_id_field(
        parser,
        emoji,
        key_id,
        &handled
    );
    if (status != DCC_OK) {
        return status;
    }
    if (!handled) {
        status = dcc_json_gateway_parse_emoji_list_text_field(
            parser,
            emoji,
            key_id,
            buffers,
            &handled
        );
        if (status != DCC_OK) {
            return status;
        }
    }
    if (!handled) {
        status = dcc_json_gateway_parse_emoji_list_user_field(
            parser,
            emoji,
            key_id,
            buffers,
            &handled
        );
        if (status != DCC_OK) {
            return status;
        }
    }
    if (!handled) {
        status = dcc_json_gateway_parse_emoji_list_bool_field(
            parser,
            emoji,
            key_id,
            &handled
        );
        if (status != DCC_OK) {
            return status;
        }
    }
    if (!handled) {
        status = dcc_json_skip_value(parser);
        if (status != DCC_OK) {
            return status;
        }
    }
    return DCC_OK;
}
