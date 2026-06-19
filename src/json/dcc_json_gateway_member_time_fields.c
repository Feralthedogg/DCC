#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_member_time_field(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (parser == NULL || out == NULL || handled == NULL) {
        return DCC_ERR_JSON;
    }

    *handled = 1;
    char *target = NULL;
    size_t target_cap = 0;
    const char **gateway_field = NULL;
    const char **member_field = NULL;

    if (key_id == DCC_JSON_KEY_JOINED_AT) {
        target = out->gateway_joined_at;
        target_cap = sizeof(out->gateway_joined_at);
        gateway_field = &out->gateway.joined_at;
        member_field = &out->member.joined_at;
    } else if (key_id == DCC_JSON_KEY_PREMIUM_SINCE) {
        target = out->gateway_premium_since;
        target_cap = sizeof(out->gateway_premium_since);
        gateway_field = &out->gateway.premium_since;
        member_field = &out->member.premium_since;
    } else if (key_id == DCC_JSON_KEY_COMMUNICATION_DISABLED_UNTIL) {
        target = out->gateway_communication_disabled_until;
        target_cap = sizeof(out->gateway_communication_disabled_until);
        gateway_field = &out->gateway.communication_disabled_until;
        member_field = &out->member.communication_disabled_until;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    int has_value = 0;
    dcc_status_t status = dcc_json_parse_nullable_string_buffer(
        parser,
        target,
        target_cap,
        &has_value
    );
    if (status != DCC_OK) {
        return status;
    }

    const char *value = has_value ? target : NULL;
    *gateway_field = value;
    *member_field = value;
    return DCC_OK;
}
