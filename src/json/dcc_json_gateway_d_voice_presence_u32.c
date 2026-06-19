#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_voice_presence_u32_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (key_id != DCC_JSON_KEY_ANSWER_ID &&
        key_id != DCC_JSON_KEY_ANIMATION_TYPE &&
        key_id != DCC_JSON_KEY_OPCODE) {
        *handled = 0;
        return DCC_OK;
    }

    uint32_t value = 0;
    dcc_status_t status = dcc_json_parse_u32_direct(parser, &value);
    if (status != DCC_OK) {
        return status;
    }

    if (key_id == DCC_JSON_KEY_ANSWER_ID) {
        out->gateway.answer_id = value;
        out->poll_vote.answer_id = value;
        out->has_poll_vote = 1;
    } else if (key_id == DCC_JSON_KEY_ANIMATION_TYPE) {
        out->gateway.animation_type = value;
        out->voice_channel_effect.animation_type = value;
        out->has_voice_channel_effect = 1;
    } else {
        out->gateway.opcode = value;
    }

    *handled = 1;
    return DCC_OK;
}
