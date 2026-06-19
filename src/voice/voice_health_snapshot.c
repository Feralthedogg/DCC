#include "internal/voice/dcc_voice_internal.h"

#include <string.h>

dcc_status_t dcc_voice_client_health_snapshot(
    const dcc_voice_client_t *voice_client,
    dcc_voice_health_snapshot_t *out
) {
    if (voice_client == NULL || out == NULL || out->size < sizeof(*out)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_voice_health_snapshot_t snapshot;
    memset(&snapshot, 0, sizeof(snapshot));
    snapshot.size = sizeof(snapshot);
    snapshot.session.size = sizeof(snapshot.session);
    snapshot.stats.size = sizeof(snapshot.stats);

    dcc_status_t status = dcc_voice_client_session_info(voice_client, &snapshot.session);
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_voice_client_stats(voice_client, &snapshot.stats);
    if (status != DCC_OK) {
        return status;
    }

    snapshot.action = dcc_voice_client_reconnect_action(voice_client);
    snapshot.health = dcc_voice_health_classify(&snapshot.session, &snapshot.stats, snapshot.action);
    snapshot.media_ready = snapshot.session.ready != 0U &&
        snapshot.stats.ssrc != 0U &&
        (snapshot.stats.encryption_mode == DCC_VOICE_ENCRYPTION_NONE || snapshot.stats.has_secret_key != 0U)
        ? 1U
        : 0U;
    snapshot.websocket_ready = snapshot.stats.websocket_loop_running != 0U &&
        snapshot.stats.websocket_loop_status == DCC_OK
        ? 1U
        : 0U;
    snapshot.udp_ready = snapshot.stats.udp_connected;
    snapshot.ok = snapshot.health == DCC_VOICE_HEALTH_READY ||
        snapshot.health == DCC_VOICE_HEALTH_ACTIVE
        ? 1U
        : 0U;
    (void)dcc_voice_copy_text(
        snapshot.reason,
        sizeof(snapshot.reason),
        dcc_voice_health_reason(&snapshot.session, &snapshot.stats, snapshot.health, snapshot.action)
    );
    *out = snapshot;
    return DCC_OK;
}
