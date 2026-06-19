#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_guild_member_entry(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    size_t index
) {
    if (parser == NULL || out == NULL) {
        return DCC_ERR_JSON;
    }

    dcc_status_t status = dcc_json_gateway_parse_member_list_object(
        parser,
        &out->gateway_members[index],
        out->gateway.guild_id,
        out->gateway_member_role_id_storage[index],
        out->gateway_member_usernames[index],
        sizeof(out->gateway_member_usernames[index]),
        out->gateway_member_user_global_names[index],
        sizeof(out->gateway_member_user_global_names[index]),
        out->gateway_member_user_avatars[index],
        sizeof(out->gateway_member_user_avatars[index]),
        out->gateway_member_nicks[index],
        sizeof(out->gateway_member_nicks[index]),
        out->gateway_member_avatars[index],
        sizeof(out->gateway_member_avatars[index]),
        out->gateway_member_joined_ats[index],
        sizeof(out->gateway_member_joined_ats[index]),
        out->gateway_member_premium_sinces[index],
        sizeof(out->gateway_member_premium_sinces[index]),
        out->gateway_member_communication_disabled_untils[index],
        sizeof(out->gateway_member_communication_disabled_untils[index]),
        NULL,
        NULL
    );
    if (status != DCC_OK) {
        return status;
    }
    if (out->gateway_members[index].user.id != 0 && index < DCC_JSON_GATEWAY_ID_LIST_CAP) {
        out->gateway_member_ids[index] = out->gateway_members[index].user.id;
    }
    return DCC_OK;
}
