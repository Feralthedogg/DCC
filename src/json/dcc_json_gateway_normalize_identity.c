#include "internal/json/dcc_json_internal.h"

#include <stddef.h>

void dcc_json_gateway_normalize_identity(dcc_json_gateway_payload_t *out) {
    int user_object_like = out->gateway.user.username != NULL ||
        out->gateway.user.global_name != NULL ||
        (out->has_event_name &&
            dcc_json_event_name_eq(
                out->event_name,
                sizeof(out->event_name),
                "USER_UPDATE",
                sizeof("USER_UPDATE") - 1
            ));

    if (user_object_like && out->gateway.user.id == 0 && out->gateway.id != 0) {
        out->gateway.user.id = out->gateway.id;
    }
    if (out->gateway.user_id == 0 && out->gateway.user.id != 0) {
        out->gateway.user_id = out->gateway.user.id;
    }
    if (user_object_like) {
        out->user = out->gateway.user;
        out->guild_ban.user = out->gateway.user;
    }
    if (out->channel.guild_id == 0 && out->gateway.guild_id != 0) {
        out->channel.guild_id = out->gateway.guild_id;
    }
    if (out->role.guild_id == 0 && out->gateway.guild_id != 0) {
        out->role.guild_id = out->gateway.guild_id;
    }
    if (out->member.guild_id == 0 && out->gateway.guild_id != 0) {
        out->member.guild_id = out->gateway.guild_id;
    }
    if (out->channel.id == 0 && out->gateway.id != 0) {
        out->channel.id = out->gateway.id;
    }
    if (out->role.id == 0 && out->gateway.role_id != 0) {
        out->role.id = out->gateway.role_id;
    }
    if (out->member.user.id == 0 && out->gateway.user.id != 0) {
        out->member.user = out->gateway.user;
    }
    if (out->member.user.id == 0 && out->gateway.user_id != 0) {
        out->member.user.id = out->gateway.user_id;
    }
    if (out->interaction_member_permissions_present &&
        out->member.user.id != 0 &&
        (out->interaction.id != 0 || out->interaction.token != NULL || out->interaction.type != 0)) {
        dcc_json_gateway_add_interaction_resolved_permission(
            out,
            out->member.user.id,
            out->interaction_member_permissions
        );
    }
}
