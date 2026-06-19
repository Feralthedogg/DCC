#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_channel_invite_text_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    int has_value = 0;

    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id == DCC_JSON_KEY_TOPIC) {
        dcc_status_t status = dcc_json_parse_nullable_string_buffer(
            parser,
            out->gateway_topic,
            sizeof(out->gateway_topic),
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.topic = has_value ? out->gateway_topic : NULL;
        out->channel.topic = has_value ? out->gateway_topic : NULL;
        out->stage_instance.topic = has_value ? out->gateway_topic : NULL;
        out->has_channel = 1;
        out->has_stage_instance = 1;
        out->cache_fields |= DCC_CACHE_FIELD_CHANNEL_TOPIC;
    } else if (key_id == DCC_JSON_KEY_CODE) {
        dcc_status_t status = dcc_json_parse_nullable_string_buffer(
            parser,
            out->gateway_code,
            sizeof(out->gateway_code),
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.code = has_value ? out->gateway_code : NULL;
        out->invite.code = has_value ? out->gateway_code : NULL;
        out->has_invite = 1;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    *handled = 1;
    return DCC_OK;
}
