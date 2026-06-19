#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_voice_presence_double_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (key_id != DCC_JSON_KEY_RETRY_AFTER && key_id != DCC_JSON_KEY_VOLUME) {
        *handled = 0;
        return DCC_OK;
    }

    double value = 0.0;
    dcc_status_t status = dcc_json_parse_double_direct(parser, &value);
    if (status != DCC_OK) {
        return status;
    }

    if (key_id == DCC_JSON_KEY_RETRY_AFTER) {
        out->gateway.retry_after = value;
    } else {
        out->gateway.volume = value;
        out->soundboard_sound.volume = value;
        out->voice_channel_effect.volume = value;
        out->has_soundboard_sound = 1;
        out->has_voice_channel_effect = 1;
    }

    *handled = 1;
    return DCC_OK;
}
