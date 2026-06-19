#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_member_context_text_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    int has_value = 0;

    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id == DCC_JSON_KEY_REASON) {
        dcc_status_t status = dcc_json_parse_nullable_string_buffer(
            parser,
            out->gateway_reason,
            sizeof(out->gateway_reason),
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.reason = has_value ? out->gateway_reason : NULL;
    } else if (key_id == DCC_JSON_KEY_NICK) {
        dcc_status_t status = dcc_json_parse_nullable_string_buffer(
            parser,
            out->gateway_nick,
            sizeof(out->gateway_nick),
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.nick = has_value ? out->gateway_nick : NULL;
        out->member.nick = has_value ? out->gateway_nick : NULL;
    } else if (key_id == DCC_JSON_KEY_NONCE) {
        dcc_status_t status = dcc_json_parse_nullable_string_buffer(
            parser,
            out->gateway_nonce,
            sizeof(out->gateway_nonce),
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.nonce = has_value ? out->gateway_nonce : NULL;
        out->guild_members_chunk.nonce = has_value ? out->gateway_nonce : NULL;
        out->has_guild_members_chunk = 1;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    *handled = 1;
    return DCC_OK;
}
