#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_resolved_member_entry(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_snowflake_t key,
    size_t index
) {
    dcc_permission_t permissions = 0;
    int has_permissions = 0;
    dcc_status_t status = dcc_json_gateway_parse_member_list_object(
        parser,
        &out->interaction_resolved_members[index],
        out->interaction.guild_id,
        out->interaction_resolved_member_role_ids[index],
        out->interaction_resolved_member_usernames[index],
        sizeof(out->interaction_resolved_member_usernames[index]),
        out->interaction_resolved_member_user_global_names[index],
        sizeof(out->interaction_resolved_member_user_global_names[index]),
        out->interaction_resolved_member_user_avatars[index],
        sizeof(out->interaction_resolved_member_user_avatars[index]),
        out->interaction_resolved_member_nicks[index],
        sizeof(out->interaction_resolved_member_nicks[index]),
        out->interaction_resolved_member_avatars[index],
        sizeof(out->interaction_resolved_member_avatars[index]),
        out->interaction_resolved_member_joined_ats[index],
        sizeof(out->interaction_resolved_member_joined_ats[index]),
        out->interaction_resolved_member_premium_sinces[index],
        sizeof(out->interaction_resolved_member_premium_sinces[index]),
        out->interaction_resolved_member_communication_disabled_untils[index],
        sizeof(out->interaction_resolved_member_communication_disabled_untils[index]),
        &permissions,
        &has_permissions
    );
    if (status != DCC_OK) {
        return status;
    }
    if (out->interaction_resolved_members[index].guild_id == 0) {
        out->interaction_resolved_members[index].guild_id = out->interaction.guild_id;
    }
    if (out->interaction_resolved_members[index].user.id == 0) {
        out->interaction_resolved_members[index].user.id = key;
    }
    if (has_permissions) {
        dcc_json_gateway_add_interaction_resolved_permission(out, key, permissions);
    }
    return DCC_OK;
}
