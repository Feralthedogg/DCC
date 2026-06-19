#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_normalize_thread_events(dcc_json_gateway_payload_t *out) {
    if (out->thread_member.id == 0 && out->gateway.id != 0) {
        out->thread_member.id = out->gateway.id;
    }
    if (out->thread_member.user_id == 0 && out->gateway.user_id != 0) {
        out->thread_member.user_id = out->gateway.user_id;
    }
    if (out->thread_member.guild_id == 0 && out->gateway.guild_id != 0) {
        out->thread_member.guild_id = out->gateway.guild_id;
    }
    if (out->thread_members_update.id == 0 && out->gateway.id != 0) {
        out->thread_members_update.id = out->gateway.id;
    }
    if (out->thread_members_update.guild_id == 0 && out->gateway.guild_id != 0) {
        out->thread_members_update.guild_id = out->gateway.guild_id;
    }
    if (out->guild_members_chunk.guild_id == 0 && out->gateway.guild_id != 0) {
        out->guild_members_chunk.guild_id = out->gateway.guild_id;
    }
}
