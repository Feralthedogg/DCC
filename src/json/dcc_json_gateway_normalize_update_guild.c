#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_normalize_update_guild_events(dcc_json_gateway_payload_t *out) {
    if (out->guild_audit_log_entry.id == 0 && out->gateway.id != 0) {
        out->guild_audit_log_entry.id = out->gateway.id;
    }
    if (out->guild_audit_log_entry.guild_id == 0 && out->gateway.guild_id != 0) {
        out->guild_audit_log_entry.guild_id = out->gateway.guild_id;
    }
    if (out->guild_audit_log_entry.user_id == 0 && out->gateway.user_id != 0) {
        out->guild_audit_log_entry.user_id = out->gateway.user_id;
    }
    if (out->guild_audit_log_entry.target_id == 0 && out->gateway.target_id != 0) {
        out->guild_audit_log_entry.target_id = out->gateway.target_id;
    }
    if (out->guild_ban.guild_id == 0 && out->gateway.guild_id != 0) {
        out->guild_ban.guild_id = out->gateway.guild_id;
    }
    if (out->guild_ban.user.id == 0 && out->gateway.user.id != 0) {
        out->guild_ban.user = out->gateway.user;
    }
    if (out->guild_ban.user.id == 0 && out->gateway.user_id != 0) {
        out->guild_ban.user.id = out->gateway.user_id;
    }
    if (out->guild_id_list_update.guild_id == 0 && out->gateway.guild_id != 0) {
        out->guild_id_list_update.guild_id = out->gateway.guild_id;
    }
    if (out->guild_update_notice.guild_id == 0 && out->gateway.guild_id != 0) {
        out->guild_update_notice.guild_id = out->gateway.guild_id;
    }
    if (out->scheduled_event_user_update.guild_id == 0 && out->gateway.guild_id != 0) {
        out->scheduled_event_user_update.guild_id = out->gateway.guild_id;
    }
    if (out->scheduled_event_user_update.scheduled_event_id == 0 &&
        out->gateway.scheduled_event_id != 0) {
        out->scheduled_event_user_update.scheduled_event_id = out->gateway.scheduled_event_id;
    }
    if (out->scheduled_event_user_update.user_id == 0 && out->gateway.user_id != 0) {
        out->scheduled_event_user_update.user_id = out->gateway.user_id;
    }
    if (out->thread_list_sync.guild_id == 0 && out->gateway.guild_id != 0) {
        out->thread_list_sync.guild_id = out->gateway.guild_id;
    }
}
