#include "internal/voice/dcc_voice_internal.h"

dcc_voice_health_t dcc_voice_health_classify(
    const dcc_voice_session_info_t *session,
    const dcc_voice_client_stats_t *stats,
    dcc_voice_reconnect_action_t action
) {
    if (session == NULL || session->size < sizeof(*session) ||
        stats == NULL || stats->size < sizeof(*stats)) {
        return DCC_VOICE_HEALTH_UNKNOWN;
    }
    if (action == DCC_VOICE_RECONNECT_ACTION_TERMINATE ||
        session->state == DCC_VOICE_SESSION_TERMINATED) {
        return DCC_VOICE_HEALTH_TERMINATED;
    }
    if (action == DCC_VOICE_RECONNECT_ACTION_RESUME ||
        action == DCC_VOICE_RECONNECT_ACTION_FULL_RECONNECT ||
        session->state == DCC_VOICE_SESSION_RECONNECTING) {
        return DCC_VOICE_HEALTH_RECONNECTING;
    }
    if (stats->websocket_loop_running != 0U && stats->websocket_loop_status != DCC_OK) {
        return DCC_VOICE_HEALTH_DEGRADED;
    }

    switch (session->state) {
        case DCC_VOICE_SESSION_DISCONNECTED:
            return DCC_VOICE_HEALTH_DISCONNECTED;
        case DCC_VOICE_SESSION_REQUESTED:
            return DCC_VOICE_HEALTH_CONNECTING;
        case DCC_VOICE_SESSION_READY:
            return session->ready != 0U ? DCC_VOICE_HEALTH_READY : DCC_VOICE_HEALTH_CONNECTING;
        case DCC_VOICE_SESSION_ACTIVE:
            return session->ready != 0U ? DCC_VOICE_HEALTH_ACTIVE : DCC_VOICE_HEALTH_DEGRADED;
        case DCC_VOICE_SESSION_TERMINATED:
            return DCC_VOICE_HEALTH_TERMINATED;
        case DCC_VOICE_SESSION_RECONNECTING:
            return DCC_VOICE_HEALTH_RECONNECTING;
        default:
            return DCC_VOICE_HEALTH_UNKNOWN;
    }
}

const char *dcc_voice_health_reason(
    const dcc_voice_session_info_t *session,
    const dcc_voice_client_stats_t *stats,
    dcc_voice_health_t health,
    dcc_voice_reconnect_action_t action
) {
    if (session == NULL || stats == NULL) {
        return "voice health unavailable";
    }
    if (stats->websocket_loop_running != 0U && stats->websocket_loop_status != DCC_OK) {
        return "voice websocket loop failed";
    }
    if (action == DCC_VOICE_RECONNECT_ACTION_FULL_RECONNECT) {
        return "voice full reconnect required";
    }
    if (action == DCC_VOICE_RECONNECT_ACTION_RESUME) {
        return "voice resume requested";
    }
    if (action == DCC_VOICE_RECONNECT_ACTION_TERMINATE) {
        return "voice session terminated";
    }

    switch (health) {
        case DCC_VOICE_HEALTH_DISCONNECTED:
            return "voice session disconnected";
        case DCC_VOICE_HEALTH_CONNECTING:
            return "voice session connecting";
        case DCC_VOICE_HEALTH_READY:
            return "voice session ready";
        case DCC_VOICE_HEALTH_ACTIVE:
            return "voice session active";
        case DCC_VOICE_HEALTH_RECONNECTING:
            return "voice reconnecting";
        case DCC_VOICE_HEALTH_DEGRADED:
            return "voice session degraded";
        case DCC_VOICE_HEALTH_TERMINATED:
            return "voice session terminated";
        case DCC_VOICE_HEALTH_UNKNOWN:
        default:
            return "voice health unavailable";
    }
}
