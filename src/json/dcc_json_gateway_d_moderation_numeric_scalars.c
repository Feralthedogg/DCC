#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_moderation_numeric_scalar_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    switch (key_id) {
        case DCC_JSON_KEY_ACTION_TYPE:
        case DCC_JSON_KEY_TRIGGER_TYPE:
        case DCC_JSON_KEY_RULE_TRIGGER_TYPE:
        case DCC_JSON_KEY_EVENT_TYPE:
            break;
        default:
            *handled = 0;
            return DCC_OK;
    }

    if (key_id == DCC_JSON_KEY_ACTION_TYPE) {
        uint32_t value = 0;
        dcc_status_t status = dcc_json_parse_u32_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.action_type = value;
        out->gateway.type = value;
        out->auto_moderation_action.action_type = value;
        out->guild_audit_log_entry.action_type = value;
        out->has_auto_moderation_action = 1;
        out->has_guild_audit_log_entry = 1;
    } else if (key_id == DCC_JSON_KEY_TRIGGER_TYPE ||
               key_id == DCC_JSON_KEY_RULE_TRIGGER_TYPE) {
        uint32_t value = 0;
        dcc_status_t status = dcc_json_parse_u32_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.trigger_type = value;
        out->auto_moderation_action.trigger_type = value;
        out->auto_moderation_rule.trigger_type = value;
        out->has_auto_moderation_action = 1;
        out->has_auto_moderation_rule = 1;
    } else if (key_id == DCC_JSON_KEY_EVENT_TYPE) {
        uint32_t value = 0;
        dcc_status_t status = dcc_json_parse_u32_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.event_type = value;
        out->auto_moderation_rule.event_type = value;
        out->has_auto_moderation_rule = 1;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    *handled = 1;
    return DCC_OK;
}
