#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_type_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out
) {
    if (parser->cur < parser->end && (*parser->cur == '"' || *parser->cur == 'n')) {
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_string_buffer(
            parser,
            out->gateway_type_text,
            sizeof(out->gateway_type_text),
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.type_text = has_value ? out->gateway_type_text : NULL;
        out->integration.type_text = has_value ? out->gateway_type_text : NULL;
        out->has_integration = 1;
        return DCC_OK;
    }

    uint32_t value = 0;
    dcc_status_t status = dcc_json_parse_u32_direct(parser, &value);
    if (status != DCC_OK) {
        return status;
    }
    out->interaction.type = value;
    out->message.type = value;
    out->channel.type = value;
    out->entitlement.type = value;
    out->gateway.type = value;
    out->has_interaction = 1;
    out->has_message = 1;
    out->has_channel = 1;
    out->has_entitlement = 1;
    out->cache_fields |= DCC_CACHE_FIELD_MESSAGE_TYPE | DCC_CACHE_FIELD_CHANNEL_TYPE;
    return DCC_OK;
}
