#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_normalize_update_core_events(dcc_json_gateway_payload_t *out) {
    if (out->channel_pins_update.guild_id == 0 && out->gateway.guild_id != 0) {
        out->channel_pins_update.guild_id = out->gateway.guild_id;
    }
    if (out->channel_pins_update.channel_id == 0 && out->gateway.channel_id != 0) {
        out->channel_pins_update.channel_id = out->gateway.channel_id;
    }
    if (out->voice_server_update.guild_id == 0 && out->gateway.guild_id != 0) {
        out->voice_server_update.guild_id = out->gateway.guild_id;
    }
    if (out->integration.id == 0 && out->gateway.id != 0) {
        out->integration.id = out->gateway.id;
    }
    if (out->integration.guild_id == 0 && out->gateway.guild_id != 0) {
        out->integration.guild_id = out->gateway.guild_id;
    }
    if (out->integration.application_id == 0 && out->gateway.application_id != 0) {
        out->integration.application_id = out->gateway.application_id;
    }
    if (out->integration.role_id == 0 && out->gateway.role_id != 0) {
        out->integration.role_id = out->gateway.role_id;
    }
    if (out->presence.guild_id == 0 && out->gateway.guild_id != 0) {
        out->presence.guild_id = out->gateway.guild_id;
    }
    if (out->presence.user.id == 0 && out->gateway.user.id != 0) {
        out->presence.user = out->gateway.user;
    }
    if (out->presence.user.id == 0 && out->gateway.user_id != 0) {
        out->presence.user.id = out->gateway.user_id;
    }
    if (out->user.id == 0 && out->gateway.user.id != 0) {
        out->user = out->gateway.user;
    }
    if (out->user.id == 0 && out->gateway.user_id != 0) {
        out->user.id = out->gateway.user_id;
    }
    if (out->application_command_permissions_update.id == 0 && out->gateway.id != 0) {
        out->application_command_permissions_update.id = out->gateway.id;
    }
    if (out->application_command_permissions_update.application_id == 0 &&
        out->gateway.application_id != 0) {
        out->application_command_permissions_update.application_id = out->gateway.application_id;
    }
    if (out->application_command_permissions_update.guild_id == 0 && out->gateway.guild_id != 0) {
        out->application_command_permissions_update.guild_id = out->gateway.guild_id;
    }
}
