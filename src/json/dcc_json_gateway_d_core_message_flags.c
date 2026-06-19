#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_core_message_flags_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id == DCC_JSON_KEY_FLAGS) {
        uint64_t value = 0;
        dcc_status_t status = dcc_json_parse_u64_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        out->message.flags = value;
        if (value <= UINT32_MAX &&
            out->has_event_name &&
            dcc_json_event_name_eq(
                out->event_name,
                sizeof(out->event_name),
                "USER_UPDATE",
                sizeof("USER_UPDATE") - 1
            )) {
            out->gateway.user.flags |= (uint32_t)value;
            out->user.flags |= (uint32_t)value;
            out->has_user = 1;
        }
        out->gateway.flags = value;
        out->guild.flags = value;
        out->member.flags = value;
        out->thread_member.flags = value;
        out->entitlement.flags = value;
        out->has_message = 1;
        out->has_guild = 1;
        out->has_thread_member = 1;
        out->has_entitlement = 1;
        out->cache_fields |= DCC_CACHE_FIELD_MESSAGE_FLAGS |
            DCC_CACHE_FIELD_GUILD_FLAGS |
            DCC_CACHE_FIELD_MEMBER_FLAGS |
            DCC_CACHE_FIELD_USER_FLAGS;
    } else if (key_id == DCC_JSON_KEY_PUBLIC_FLAGS) {
        uint32_t value = 0;
        dcc_status_t status = dcc_json_parse_u32_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.user.flags |= value;
        out->user.flags |= value;
        out->has_user = 1;
        out->cache_fields |= DCC_CACHE_FIELD_USER_FLAGS;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    *handled = 1;
    return DCC_OK;
}
