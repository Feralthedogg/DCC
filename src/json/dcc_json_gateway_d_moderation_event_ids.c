#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_moderation_event_id_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id == DCC_JSON_KEY_RULE_ID) {
        uint64_t value = 0;
        dcc_status_t status = dcc_json_parse_u64_token(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.rule_id = value;
        out->auto_moderation_action.rule_id = value;
        out->auto_moderation_rule.id = value;
        out->has_auto_moderation_action = 1;
        out->has_auto_moderation_rule = 1;
    } else if (key_id == DCC_JSON_KEY_ALERT_SYSTEM_MESSAGE_ID) {
        uint64_t value = 0;
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.alert_system_message_id = value;
        out->auto_moderation_action.alert_system_message_id = value;
        out->has_auto_moderation_action = 1;
        (void)has_value;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    *handled = 1;
    return DCC_OK;
}
