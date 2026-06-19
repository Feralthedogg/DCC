#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_voice_presence_time_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (key_id == DCC_JSON_KEY_VOICE_START_TIME) {
        uint64_t value = 0;
        dcc_status_t status = dcc_json_parse_u64_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.timestamp = value;
        out->voice_channel_start_time_update.voice_start_time = value;
        out->has_voice_channel_start_time_update = 1;
        *handled = 1;
        return DCC_OK;
    }
    if (key_id == DCC_JSON_KEY_SINCE) {
        uint64_t value = 0;
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.since = value;
        out->presence.since = value;
        out->has_presence = 1;
        (void)has_value;
        *handled = 1;
        return DCC_OK;
    }

    *handled = 0;
    return DCC_OK;
}
