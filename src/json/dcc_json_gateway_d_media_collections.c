#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_media_collection_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    *handled = 1;
    if (key_id == DCC_JSON_KEY_EMOJI) {
        dcc_status_t status = dcc_json_gateway_parse_emoji(parser, out);
        if (status != DCC_OK) {
            return status;
        }
    } else if (key_id == DCC_JSON_KEY_ACTIVITIES) {
        dcc_status_t status = dcc_json_gateway_parse_activities(parser, out);
        if (status != DCC_OK) {
            return status;
        }
    } else if (key_id == DCC_JSON_KEY_EMOJIS) {
        dcc_status_t status = dcc_json_gateway_parse_emoji_array(parser, out);
        if (status != DCC_OK) {
            return status;
        }
    } else if (key_id == DCC_JSON_KEY_STICKERS) {
        dcc_status_t status = dcc_json_gateway_parse_sticker_array(parser, out);
        if (status != DCC_OK) {
            return status;
        }
    } else if (key_id == DCC_JSON_KEY_SOUNDBOARD_SOUNDS) {
        dcc_status_t status = dcc_json_gateway_parse_soundboard_array(parser, out);
        if (status != DCC_OK) {
            return status;
        }
    } else {
        *handled = 0;
    }
    return DCC_OK;
}
