#include "internal/json/dcc_json_internal.h"

#include <string.h>

dcc_status_t dcc_json_gateway_parse_d_status_text_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (key_id != DCC_JSON_KEY_STATUS) {
        *handled = 0;
        return DCC_OK;
    }

    int has_value = 0;
    dcc_status_t status = dcc_json_parse_nullable_string_or_number_buffer(
        parser,
        out->gateway_status,
        sizeof(out->gateway_status),
        &has_value
    );
    if (status != DCC_OK) {
        return status;
    }

    out->gateway.status = has_value ? out->gateway_status : NULL;
    out->subscription.status = has_value ? out->gateway_status : NULL;
    out->presence.status = has_value ? out->gateway_status : NULL;
    out->voice_channel_status_update.status = has_value ? out->gateway_status : NULL;
    out->guild_join_request_update.status = has_value ? out->gateway_status : NULL;
    if (has_value) {
        uint64_t status_value = 0;
        size_t status_len = strlen(out->gateway_status);
        if (dcc_json_parse_u64_ascii(out->gateway_status, status_len, &status_value) == 0 &&
            status_value <= UINT32_MAX) {
            out->scheduled_event.status = (uint32_t)status_value;
            out->has_scheduled_event = 1;
        }
    }
    out->has_subscription = 1;
    out->has_presence = 1;
    out->has_voice_channel_status_update = 1;
    out->has_guild_join_request_update = 1;

    *handled = 1;
    return DCC_OK;
}
