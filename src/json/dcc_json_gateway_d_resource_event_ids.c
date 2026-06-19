#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_resource_event_id_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id == DCC_JSON_KEY_MESSAGE_AUTHOR_ID) {
        uint64_t value = 0;
        dcc_status_t status = dcc_json_parse_u64_token(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.message_author_id = value;
    } else if (key_id == DCC_JSON_KEY_ENTITY_ID) {
        uint64_t value = 0;
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.entity_id = value;
        out->scheduled_event.entity_id = value;
        out->has_scheduled_event = 1;
        (void)has_value;
    } else if (key_id == DCC_JSON_KEY_CREATOR_ID) {
        uint64_t value = 0;
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.creator_id = value;
        out->scheduled_event.creator_id = value;
        out->auto_moderation_rule.creator_id = value;
        out->has_scheduled_event = 1;
        out->has_auto_moderation_rule = 1;
        (void)has_value;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    *handled = 1;
    return DCC_OK;
}
