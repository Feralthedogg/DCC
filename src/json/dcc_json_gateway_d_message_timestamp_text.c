#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_message_edited_timestamp_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out
) {
    int has_value = 0;
    dcc_status_t status = dcc_json_parse_nullable_string_buffer(
        parser,
        out->message_edited_timestamp,
        sizeof(out->message_edited_timestamp),
        &has_value
    );
    if (status != DCC_OK) {
        return status;
    }
    out->message.edited_timestamp = has_value ? out->message_edited_timestamp : NULL;
    out->has_message = 1;
    out->cache_fields |= DCC_CACHE_FIELD_MESSAGE_EDITED_TIMESTAMP;
    return DCC_OK;
}

dcc_status_t dcc_json_gateway_parse_d_last_pin_timestamp_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out
) {
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
    out->channel_pins_update.last_pin_timestamp = has_value ? out->gateway_timestamp_text : NULL;
    out->has_channel_pins_update = 1;
    return DCC_OK;
}
