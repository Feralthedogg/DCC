#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_member_text_field(
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

    if (key_id == DCC_JSON_KEY_NICK) {
        target = out->gateway_nick;
        target_cap = sizeof(out->gateway_nick);
        gateway_field = &out->gateway.nick;
        member_field = &out->member.nick;
    } else if (key_id == DCC_JSON_KEY_AVATAR) {
        target = out->gateway_avatar;
        target_cap = sizeof(out->gateway_avatar);
        gateway_field = &out->gateway.avatar;
        member_field = &out->member.avatar;
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
