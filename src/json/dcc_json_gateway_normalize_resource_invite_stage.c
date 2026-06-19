#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_normalize_invite_stage_resources(dcc_json_gateway_payload_t *out) {
    if (out->invite.guild_id == 0 && out->gateway.guild_id != 0) {
        out->invite.guild_id = out->gateway.guild_id;
    }
    if (out->invite.channel_id == 0 && out->gateway.channel_id != 0) {
        out->invite.channel_id = out->gateway.channel_id;
    }
    if (out->invite.scheduled_event_id == 0 && out->gateway.scheduled_event_id != 0) {
        out->invite.scheduled_event_id = out->gateway.scheduled_event_id;
    }
    if (out->invite.inviter.id == 0 && out->gateway.user.id != 0) {
        out->invite.inviter = out->gateway.user;
    }
    if (out->stage_instance.guild_id == 0 && out->gateway.guild_id != 0) {
        out->stage_instance.guild_id = out->gateway.guild_id;
    }
    if (out->stage_instance.channel_id == 0 && out->gateway.channel_id != 0) {
        out->stage_instance.channel_id = out->gateway.channel_id;
    }
    if (out->stage_instance.scheduled_event_id == 0 && out->gateway.scheduled_event_id != 0) {
        out->stage_instance.scheduled_event_id = out->gateway.scheduled_event_id;
    }
}
