#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_voice_flag_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    switch (key_id) {
        case DCC_JSON_KEY_DEAF:
        case DCC_JSON_KEY_MUTE:
        case DCC_JSON_KEY_SELF_DEAF:
        case DCC_JSON_KEY_SELF_MUTE:
        case DCC_JSON_KEY_SELF_STREAM:
        case DCC_JSON_KEY_SELF_VIDEO:
        case DCC_JSON_KEY_SUPPRESS:
            break;
        default:
            *handled = 0;
            return DCC_OK;
    }

    int value = 0;
    dcc_status_t status = dcc_json_parse_bool_direct(parser, &value);
    if (status != DCC_OK) {
        return status;
    }

    if (key_id == DCC_JSON_KEY_DEAF) {
        out->voice_state.deaf = value ? 1U : 0U;
        out->gateway.deaf = value ? 1U : 0U;
    } else if (key_id == DCC_JSON_KEY_MUTE) {
        out->voice_state.mute = value ? 1U : 0U;
        out->gateway.mute = value ? 1U : 0U;
    } else if (key_id == DCC_JSON_KEY_SELF_DEAF) {
        out->voice_state.self_deaf = value ? 1U : 0U;
    } else if (key_id == DCC_JSON_KEY_SELF_MUTE) {
        out->voice_state.self_mute = value ? 1U : 0U;
    } else if (key_id == DCC_JSON_KEY_SELF_STREAM) {
        out->voice_state.self_stream = value ? 1U : 0U;
    } else if (key_id == DCC_JSON_KEY_SELF_VIDEO) {
        out->voice_state.self_video = value ? 1U : 0U;
    } else if (key_id == DCC_JSON_KEY_SUPPRESS) {
        out->voice_state.suppress = value ? 1U : 0U;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    out->has_voice_state = 1;
    out->cache_fields |= DCC_CACHE_FIELD_VOICE_FLAGS;
    *handled = 1;
    return DCC_OK;
}
