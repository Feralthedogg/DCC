#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_core_message_bool_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    int value = 0;
    uint64_t cache_field = 0;
    uint8_t *target = NULL;

    if (key_id == DCC_JSON_KEY_TTS) {
        target = &out->message.tts;
        cache_field = DCC_CACHE_FIELD_MESSAGE_TTS;
    } else if (key_id == DCC_JSON_KEY_MENTION_EVERYONE) {
        target = &out->message.mention_everyone;
        cache_field = DCC_CACHE_FIELD_MESSAGE_MENTION_EVERYONE;
    } else if (key_id == DCC_JSON_KEY_PINNED) {
        target = &out->message.pinned;
        cache_field = DCC_CACHE_FIELD_MESSAGE_PINNED;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    dcc_status_t status = dcc_json_parse_bool_direct(parser, &value);
    if (status != DCC_OK) {
        return status;
    }
    *target = value ? 1U : 0U;
    out->has_message = 1;
    out->cache_fields |= cache_field;
    *handled = 1;
    return DCC_OK;
}
