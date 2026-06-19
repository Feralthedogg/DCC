#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_core_user_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id == DCC_JSON_KEY_USERNAME) {
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_string_buffer(
            parser,
            out->gateway_user_username,
            sizeof(out->gateway_user_username),
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.user.username = has_value ? out->gateway_user_username : NULL;
    } else if (key_id == DCC_JSON_KEY_GLOBAL_NAME) {
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_string_buffer(
            parser,
            out->gateway_user_global_name,
            sizeof(out->gateway_user_global_name),
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.user.global_name = has_value ? out->gateway_user_global_name : NULL;
    } else if (key_id == DCC_JSON_KEY_DISCRIMINATOR) {
        uint16_t value = 0;
        dcc_status_t status = dcc_json_parse_u16_token(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.user.discriminator = value;
    } else if (key_id == DCC_JSON_KEY_BOT) {
        int value = 0;
        dcc_status_t status = dcc_json_parse_bool_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.user.bot = value ? 1U : 0U;
        out->cache_fields |= DCC_CACHE_FIELD_USER_BOT;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    *handled = 1;
    return DCC_OK;
}
