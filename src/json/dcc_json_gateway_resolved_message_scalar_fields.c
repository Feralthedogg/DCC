#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_resolved_message_scalar_field(
    dcc_json_parser_t *parser,
    dcc_message_t *message,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (parser == NULL || message == NULL || handled == NULL) {
        return DCC_ERR_JSON;
    }

    *handled = 1;
    if (key_id == DCC_JSON_KEY_TYPE) {
        uint32_t value = 0;
        dcc_status_t status = dcc_json_parse_u32_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        message->type = value;
    } else if (key_id == DCC_JSON_KEY_FLAGS) {
        uint64_t value = 0;
        dcc_status_t status = dcc_json_parse_u64_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        message->flags = value;
    } else if (key_id == DCC_JSON_KEY_TTS ||
               key_id == DCC_JSON_KEY_MENTION_EVERYONE ||
               key_id == DCC_JSON_KEY_PINNED) {
        int value = 0;
        dcc_status_t status = dcc_json_parse_bool_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        if (key_id == DCC_JSON_KEY_TTS) {
            message->tts = value ? 1U : 0U;
        } else if (key_id == DCC_JSON_KEY_MENTION_EVERYONE) {
            message->mention_everyone = value ? 1U : 0U;
        } else {
            message->pinned = value ? 1U : 0U;
        }
    } else {
        *handled = 0;
    }
    return DCC_OK;
}
