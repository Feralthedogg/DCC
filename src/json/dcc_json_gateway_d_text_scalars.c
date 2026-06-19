#include "internal/json/dcc_json_internal.h"

static int dcc_json_gateway_is_d_text_scalar_key(dcc_json_key_id_t key_id) {
    switch (key_id) {
        case DCC_JSON_KEY_CODE:
        case DCC_JSON_KEY_EMOJI_NAME:
        case DCC_JSON_KEY_ENDPOINT:
        case DCC_JSON_KEY_MATCHED_CONTENT:
        case DCC_JSON_KEY_MATCHED_KEYWORD:
        case DCC_JSON_KEY_NICK:
        case DCC_JSON_KEY_NONCE:
        case DCC_JSON_KEY_REASON:
        case DCC_JSON_KEY_STATUS:
        case DCC_JSON_KEY_TOPIC:
            return 1;
        default:
            return 0;
    }
}

dcc_status_t dcc_json_gateway_parse_d_text_scalar_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (!dcc_json_gateway_is_d_text_scalar_key(key_id)) {
        *handled = 0;
        return DCC_OK;
    }

    dcc_status_t status = dcc_json_gateway_parse_d_channel_invite_text_key(
        parser,
        out,
        key_id,
        handled
    );
    if (status != DCC_OK || *handled) {
        return status;
    }

    status = dcc_json_gateway_parse_d_status_text_key(parser, out, key_id, handled);
    if (status != DCC_OK || *handled) {
        return status;
    }

    status = dcc_json_gateway_parse_d_emoji_voice_text_key(parser, out, key_id, handled);
    if (status != DCC_OK || *handled) {
        return status;
    }

    return dcc_json_gateway_parse_d_context_text_key(parser, out, key_id, handled);
}
