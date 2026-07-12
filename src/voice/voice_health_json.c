#include "internal/voice/dcc_voice_internal.h"

#include <stddef.h>

dcc_status_t dcc_voice_client_health_snapshot_json(
    const dcc_voice_health_snapshot_t *snapshot,
    char *out,
    size_t out_size,
    size_t *out_len
) {
    if (snapshot == NULL || snapshot->size < offsetof(dcc_voice_health_snapshot_t, dave) ||
        out == NULL || out_size == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_voice_dave_stats_t dave = {0};
    if (snapshot->size >= offsetof(dcc_voice_health_snapshot_t, dave) + sizeof(snapshot->dave)) {
        dave = snapshot->dave;
    }

    dcc_voice_json_buffer_t json = {
        .data = out,
        .len = 0,
        .cap = out_size,
        .overflow = 0,
    };
    if (dcc_voice_json_append_cstr(&json, "{\"ok\":") != 0 ||
        dcc_voice_json_append_cstr(&json, snapshot->ok != 0U ? "true" : "false") != 0 ||
        dcc_voice_json_append_cstr(&json, ",\"health\":") != 0 ||
        dcc_voice_json_append_string(&json, dcc_voice_health_string(snapshot->health)) != 0 ||
        dcc_voice_json_append_cstr(&json, ",\"action\":") != 0 ||
        dcc_voice_json_append_string(&json, dcc_voice_reconnect_action_string(snapshot->action)) != 0 ||
        dcc_voice_json_append_cstr(&json, ",\"state\":") != 0 ||
        dcc_voice_json_append_string(&json, dcc_voice_session_state_string(snapshot->session.state)) != 0 ||
        dcc_voice_json_append_cstr(&json, ",\"media_ready\":") != 0 ||
        dcc_voice_json_append_cstr(&json, snapshot->media_ready != 0U ? "true" : "false") != 0 ||
        dcc_voice_json_append_cstr(&json, ",\"websocket_ready\":") != 0 ||
        dcc_voice_json_append_cstr(&json, snapshot->websocket_ready != 0U ? "true" : "false") != 0 ||
        dcc_voice_json_append_cstr(&json, ",\"udp_ready\":") != 0 ||
        dcc_voice_json_append_cstr(&json, snapshot->udp_ready != 0U ? "true" : "false") != 0 ||
        dcc_voice_json_append_cstr(&json, ",\"guild_id\":\"") != 0 ||
        dcc_voice_json_append_u64(&json, snapshot->session.guild_id) != 0 ||
        dcc_voice_json_append_cstr(&json, "\",\"channel_id\":\"") != 0 ||
        dcc_voice_json_append_u64(&json, snapshot->session.channel_id) != 0 ||
        dcc_voice_json_append_cstr(&json, "\",\"last_close_code\":") != 0 ||
        dcc_voice_json_append_u64(&json, snapshot->session.last_close_code) != 0 ||
        dcc_voice_json_append_cstr(&json, ",\"reconnect_attempts\":") != 0 ||
        dcc_voice_json_append_u64(&json, snapshot->stats.reconnect_attempts) != 0 ||
        dcc_voice_json_append_cstr(&json, ",\"send_queue_size\":") != 0 ||
        dcc_voice_json_append_u64(&json, snapshot->stats.send_queue_size) != 0 ||
        dcc_voice_json_append_cstr(&json, ",\"receive_packets_lost\":") != 0 ||
        dcc_voice_json_append_u64(&json, snapshot->stats.receive_packets_lost) != 0 ||
        dcc_voice_json_append_cstr(&json, ",\"receive_packets_dropped\":") != 0 ||
        dcc_voice_json_append_u64(&json, snapshot->stats.receive_packets_dropped) != 0 ||
        dcc_voice_json_append_cstr(&json, ",\"dave_enabled\":") != 0 ||
        dcc_voice_json_append_cstr(&json, snapshot->stats.dave_enabled != 0U ? "true" : "false") != 0 ||
        dcc_voice_json_append_cstr(&json, ",\"dave_transition_pending\":") != 0 ||
        dcc_voice_json_append_cstr(&json, snapshot->stats.dave_transition_pending != 0U ? "true" : "false") != 0 ||
        dcc_voice_json_append_cstr(&json, ",\"dave_backend_available\":") != 0 ||
        dcc_voice_json_append_cstr(&json, dave.backend_available != 0U ? "true" : "false") != 0 ||
        dcc_voice_json_append_cstr(&json, ",\"dave_media_ready\":") != 0 ||
        dcc_voice_json_append_cstr(&json, dave.media_ready != 0U ? "true" : "false") != 0 ||
        dcc_voice_json_append_cstr(&json, ",\"dave_epoch\":") != 0 ||
        dcc_voice_json_append_u64(&json, dave.epoch) != 0 ||
        dcc_voice_json_append_cstr(&json, ",\"dave_participant_count\":") != 0 ||
        dcc_voice_json_append_u64(&json, dave.participant_count) != 0 ||
        dcc_voice_json_append_cstr(&json, ",\"dave_encrypt_success\":") != 0 ||
        dcc_voice_json_append_u64(&json, dave.encrypt_success) != 0 ||
        dcc_voice_json_append_cstr(&json, ",\"dave_encrypt_failures\":") != 0 ||
        dcc_voice_json_append_u64(&json, dave.encrypt_failures) != 0 ||
        dcc_voice_json_append_cstr(&json, ",\"dave_decrypt_success\":") != 0 ||
        dcc_voice_json_append_u64(&json, dave.decrypt_success) != 0 ||
        dcc_voice_json_append_cstr(&json, ",\"dave_decrypt_failures\":") != 0 ||
        dcc_voice_json_append_u64(&json, dave.decrypt_failures) != 0 ||
        dcc_voice_json_append_cstr(&json, ",\"dave_missing_key_failures\":") != 0 ||
        dcc_voice_json_append_u64(&json, dave.missing_key_failures) != 0 ||
        dcc_voice_json_append_cstr(&json, ",\"reason\":") != 0 ||
        dcc_voice_json_append_string(&json, snapshot->reason) != 0 ||
        dcc_voice_json_append_cstr(&json, "}") != 0) {
        return dcc_voice_json_finish(&json, out_len);
    }
    return dcc_voice_json_finish(&json, out_len);
}
