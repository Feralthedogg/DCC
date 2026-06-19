#include "internal/voice/dcc_voice_internal.h"

#include <limits.h>
#include <string.h>

dcc_status_t dcc_voice_client_session_info(
    const dcc_voice_client_t *voice_client,
    dcc_voice_session_info_t *out
) {
    if (voice_client == NULL || out == NULL || out->size < sizeof(*out)) {
        return DCC_ERR_INVALID_ARG;
    }

    size_t size = out->size;
    memset(out, 0, sizeof(*out));
    out->size = size;
    out->state = voice_client->session_state;
    out->guild_id = voice_client->guild_id;
    out->channel_id = voice_client->channel_id;
    out->session_id = voice_client->session_id;
    out->token = voice_client->session_token;
    out->endpoint = voice_client->endpoint;
    out->self_mute = voice_client->self_mute;
    out->self_deaf = voice_client->self_deaf;
    out->dave_enabled = voice_client->dave_enabled;
    out->dave_version = voice_client->dave_version;
    out->ready = dcc_voice_session_descriptor_ready(voice_client);
    out->active = voice_client->session_state == DCC_VOICE_SESSION_ACTIVE ? 1U : 0U;
    out->reconnect_requested = voice_client->reconnect_requested;
    out->full_reconnect_requested = voice_client->full_reconnect_requested;
    out->reconnect_attempts = voice_client->reconnect_attempts;
    out->last_close_code = voice_client->last_close_code;
    out->last_close_reason = voice_client->last_close_reason;
    out->heartbeat_interval_ms = voice_client->heartbeat_interval_ms;
    out->dave_transition_id = voice_client->dave_transition_id;
    out->dave_pending_version = voice_client->dave_pending_version;
    out->dave_transition_pending = voice_client->dave_transition_pending;
    out->dave_transition_ready = voice_client->dave_transition_ready;
    out->receive_sequence = voice_client->receive_sequence;
    out->udp_host = voice_client->udp_host;
    out->udp_port = voice_client->udp_port;
    out->discovered_address = voice_client->discovered_address;
    out->discovered_port = voice_client->discovered_port;
    out->select_protocol_sent = voice_client->select_protocol_sent;
    return DCC_OK;
}

dcc_status_t dcc_voice_client_stats(
    const dcc_voice_client_t *voice_client,
    dcc_voice_client_stats_t *out
) {
    if (voice_client == NULL || out == NULL || out->size < sizeof(*out)) {
        return DCC_ERR_INVALID_ARG;
    }

    size_t size = out->size;
    memset(out, 0, sizeof(*out));
    out->size = size;
    out->state = voice_client->session_state;
    out->ready = dcc_voice_session_descriptor_ready(voice_client);
    out->active = voice_client->session_state == DCC_VOICE_SESSION_ACTIVE ? 1U : 0U;
    out->reconnect_requested = voice_client->reconnect_requested;
    out->full_reconnect_requested = voice_client->full_reconnect_requested;
    out->reconnect_attempts = voice_client->reconnect_attempts;
    out->last_close_code = voice_client->last_close_code;
    out->heartbeat_interval_ms = voice_client->heartbeat_interval_ms;
    out->receive_sequence = voice_client->receive_sequence;
    out->send_queue_size = voice_client->send_queue_len;
    if (voice_client->send_queue_len != 0) {
        size_t tracks = dcc_voice_send_queue_marker_count(voice_client) + 1U;
        out->tracks_remaining = tracks > UINT32_MAX ? UINT32_MAX : (uint32_t)tracks;
    }
    out->mixed_source_count = voice_client->mix_source_count;
    out->receive_packets_lost = voice_client->receive_packets_lost;
    out->receive_packets_dropped = voice_client->receive_packets_dropped;
    out->receive_reorder_window = voice_client->receive_reorder_window;
    out->receive_reorder_buffered = dcc_voice_reorder_count(voice_client);
    out->receive_jitter_delay_ms = voice_client->receive_jitter_delay_ms;
    out->receive_jitter_timer_running =
        atomic_load_explicit(&voice_client->receive_jitter_timer_running, memory_order_acquire) ? 1U : 0U;
    out->receive_jitter_timer_interval_ms = voice_client->receive_jitter_timer_interval_ms;
    out->websocket_loop_running =
        atomic_load_explicit(&voice_client->websocket_task_running, memory_order_acquire) ? 1U : 0U;
    out->websocket_loop_status =
        (dcc_status_t)atomic_load_explicit(&voice_client->websocket_last_status, memory_order_acquire);
    out->udp_connected = LLAM_FD_IS_INVALID(voice_client->udp_fd) ? 0U : 1U;
    out->ssrc = voice_client->ssrc;
    out->rtp_sequence = voice_client->rtp_sequence;
    out->rtp_timestamp = voice_client->rtp_timestamp;
    out->encryption_mode = voice_client->encryption_mode;
    out->has_secret_key = voice_client->has_secret_key;
    out->packet_nonce = voice_client->packet_nonce;
    out->select_protocol_sent = voice_client->select_protocol_sent;
    out->dave_enabled = voice_client->dave_enabled;
    out->dave_version = voice_client->dave_version;
    out->dave_transition_pending = voice_client->dave_transition_pending;
    out->dave_transition_ready = voice_client->dave_transition_ready;
    out->dave_transition_id = voice_client->dave_transition_id;
    out->dave_pending_version = voice_client->dave_pending_version;
    return DCC_OK;
}
