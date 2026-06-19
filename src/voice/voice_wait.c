#include "internal/voice/dcc_voice_internal.h"

static int dcc_voice_wait_ready_target(const dcc_voice_health_snapshot_t *snapshot) {
    return snapshot->ok != 0U;
}

static int dcc_voice_wait_active_target(const dcc_voice_health_snapshot_t *snapshot) {
    return snapshot->health == DCC_VOICE_HEALTH_ACTIVE && snapshot->session.active != 0U;
}

static int dcc_voice_wait_terminal(const dcc_voice_health_snapshot_t *snapshot) {
    return snapshot->health == DCC_VOICE_HEALTH_DISCONNECTED ||
        snapshot->health == DCC_VOICE_HEALTH_DEGRADED ||
        snapshot->health == DCC_VOICE_HEALTH_TERMINATED;
}

static dcc_status_t dcc_voice_wait_until(
    dcc_voice_client_t *voice_client,
    uint32_t timeout_ms,
    dcc_voice_health_snapshot_t *out,
    int (*target)(const dcc_voice_health_snapshot_t *)
) {
    if (voice_client == NULL || target == NULL || (out != NULL && out->size < sizeof(*out))) {
        return DCC_ERR_INVALID_ARG;
    }

    uint64_t start_ms = dcc_voice_now_ms();
    for (;;) {
        dcc_voice_health_snapshot_t snapshot = {
            .size = sizeof(snapshot),
        };
        dcc_status_t status = dcc_voice_client_health_snapshot(voice_client, &snapshot);
        if (status != DCC_OK) {
            return status;
        }
        if (out != NULL) {
            *out = snapshot;
        }
        if (target(&snapshot)) {
            return DCC_OK;
        }
        if (dcc_voice_wait_terminal(&snapshot)) {
            return DCC_ERR_STATE;
        }
        if (timeout_ms != 0U && dcc_voice_now_ms() - start_ms >= timeout_ms) {
            return DCC_ERR_TIMEOUT;
        }
        dcc_voice_sleep_ms(1U);
    }
}

dcc_status_t dcc_voice_client_wait_until_ready(
    dcc_voice_client_t *voice_client,
    uint32_t timeout_ms,
    dcc_voice_health_snapshot_t *out
) {
    return dcc_voice_wait_until(voice_client, timeout_ms, out, dcc_voice_wait_ready_target);
}

dcc_status_t dcc_voice_client_wait_until_active(
    dcc_voice_client_t *voice_client,
    uint32_t timeout_ms,
    dcc_voice_health_snapshot_t *out
) {
    return dcc_voice_wait_until(voice_client, timeout_ms, out, dcc_voice_wait_active_target);
}
