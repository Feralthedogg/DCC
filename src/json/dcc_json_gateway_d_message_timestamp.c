#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_message_timestamp_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out
) {
    if (parser->cur < parser->end && (*parser->cur == '"' || *parser->cur == 'n')) {
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_string_buffer(
            parser,
            out->gateway_timestamp_text,
            sizeof(out->gateway_timestamp_text),
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.timestamp_text = has_value ? out->gateway_timestamp_text : NULL;
        out->message.timestamp = has_value ? out->gateway_timestamp_text : NULL;
        out->channel_pins_update.last_pin_timestamp = has_value ? out->gateway_timestamp_text : NULL;
        out->has_message = 1;
        out->has_channel_pins_update = 1;
        out->cache_fields |= DCC_CACHE_FIELD_MESSAGE_TIMESTAMP;
        return DCC_OK;
    }

    uint64_t value = 0;
    dcc_status_t status = dcc_json_parse_u64_direct(parser, &value);
    if (status != DCC_OK) {
        return status;
    }
    out->gateway.timestamp = value;
    return DCC_OK;
}
