#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_invite_thread_time_scalar_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    int has_value = 0;

    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id == DCC_JSON_KEY_JOIN_TIMESTAMP) {
        dcc_status_t status = dcc_json_parse_nullable_string_buffer(
            parser,
            out->gateway_join_timestamp,
            sizeof(out->gateway_join_timestamp),
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.join_timestamp = has_value ? out->gateway_join_timestamp : NULL;
        out->thread_member.join_timestamp = has_value ? out->gateway_join_timestamp : NULL;
        out->has_thread_member = 1;
    }
    else if (key_id == DCC_JSON_KEY_CREATED_AT) {
        dcc_status_t status = dcc_json_parse_nullable_string_buffer(
            parser,
            out->gateway_created_at,
            sizeof(out->gateway_created_at),
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.created_at = has_value ? out->gateway_created_at : NULL;
        out->invite.created_at = has_value ? out->gateway_created_at : NULL;
        out->has_invite = 1;
    }
    else if (key_id == DCC_JSON_KEY_EXPIRES_AT) {
        dcc_status_t status = dcc_json_parse_nullable_string_buffer(
            parser,
            out->gateway_expires_at,
            sizeof(out->gateway_expires_at),
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.expires_at = has_value ? out->gateway_expires_at : NULL;
        out->invite.expires_at = has_value ? out->gateway_expires_at : NULL;
        out->has_invite = 1;
    }
    else {
        *handled = 0;
        return DCC_OK;
    }

    *handled = 1;
    return DCC_OK;
}
