#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_member_time_scalar_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    int has_value = 0;

    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id == DCC_JSON_KEY_JOINED_AT) {
        dcc_status_t status = dcc_json_parse_nullable_string_buffer(
            parser,
            out->gateway_joined_at,
            sizeof(out->gateway_joined_at),
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.joined_at = has_value ? out->gateway_joined_at : NULL;
        out->member.joined_at = has_value ? out->gateway_joined_at : NULL;
    } else if (key_id == DCC_JSON_KEY_PREMIUM_SINCE) {
        dcc_status_t status = dcc_json_parse_nullable_string_buffer(
            parser,
            out->gateway_premium_since,
            sizeof(out->gateway_premium_since),
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.premium_since = has_value ? out->gateway_premium_since : NULL;
        out->member.premium_since = has_value ? out->gateway_premium_since : NULL;
    } else if (key_id == DCC_JSON_KEY_COMMUNICATION_DISABLED_UNTIL) {
        dcc_status_t status = dcc_json_parse_nullable_string_buffer(
            parser,
            out->gateway_communication_disabled_until,
            sizeof(out->gateway_communication_disabled_until),
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.communication_disabled_until = has_value
            ? out->gateway_communication_disabled_until
            : NULL;
        out->member.communication_disabled_until = has_value
            ? out->gateway_communication_disabled_until
            : NULL;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    *handled = 1;
    return DCC_OK;
}
