#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_guild_avatar_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (key_id != DCC_JSON_KEY_AVATAR) {
        *handled = 0;
        return DCC_OK;
    }

    int has_value = 0;
    dcc_status_t status = dcc_json_parse_nullable_string_buffer(
        parser,
        out->gateway_avatar,
        sizeof(out->gateway_avatar),
        &has_value
    );
    if (status != DCC_OK) {
        return status;
    }

    out->gateway.avatar = has_value ? out->gateway_avatar : NULL;
    out->member.avatar = has_value ? out->gateway_avatar : NULL;
    if (out->has_event_name &&
        dcc_json_event_name_eq(
            out->event_name,
            sizeof(out->event_name),
            "USER_UPDATE",
            sizeof("USER_UPDATE") - 1
        )) {
        out->gateway.user.avatar = has_value ? out->gateway_avatar : NULL;
        out->user.avatar = has_value ? out->gateway_avatar : NULL;
        out->has_user = 1;
    }

    *handled = 1;
    return DCC_OK;
}
