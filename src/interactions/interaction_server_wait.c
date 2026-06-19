#include "internal/interactions/dcc_interaction_server_internal.h"

#include <llam/io.h>

static int dcc_interaction_server_wait_ready_target(
    const dcc_interaction_server_health_snapshot_t *snapshot
) {
    return snapshot->ok != 0U;
}

static int dcc_interaction_server_wait_drained_target(
    const dcc_interaction_server_health_snapshot_t *snapshot
) {
    return snapshot->drain_complete != 0U;
}

static int dcc_interaction_server_wait_ready_terminal(
    const dcc_interaction_server_health_snapshot_t *snapshot
) {
    return snapshot->health == DCC_INTERACTION_SERVER_HEALTH_DEGRADED ||
        snapshot->health == DCC_INTERACTION_SERVER_HEALTH_DRAINING ||
        snapshot->health == DCC_INTERACTION_SERVER_HEALTH_STOPPED ||
        snapshot->health == DCC_INTERACTION_SERVER_HEALTH_UNKNOWN;
}

static int dcc_interaction_server_wait_drained_terminal(
    const dcc_interaction_server_health_snapshot_t *snapshot
) {
    return snapshot->health == DCC_INTERACTION_SERVER_HEALTH_STOPPED &&
        snapshot->drain_complete == 0U;
}

static dcc_status_t dcc_interaction_server_wait_until(
    dcc_interaction_server_t *server,
    uint32_t timeout_ms,
    dcc_interaction_server_health_snapshot_t *out,
    int (*target)(const dcc_interaction_server_health_snapshot_t *),
    int (*terminal)(const dcc_interaction_server_health_snapshot_t *)
) {
    if (server == NULL || target == NULL || terminal == NULL ||
        (out != NULL && out->size < sizeof(*out))) {
        return DCC_ERR_INVALID_ARG;
    }

    const uint64_t timeout_ns = (uint64_t)timeout_ms * UINT64_C(1000000);
    const uint64_t start_ns = llam_now_ns();
    for (;;) {
        dcc_interaction_server_health_snapshot_t snapshot = {
            .size = sizeof(snapshot),
        };
        dcc_status_t status = dcc_interaction_server_health_snapshot(server, &snapshot);
        if (status != DCC_OK) {
            return status;
        }
        if (out != NULL) {
            *out = snapshot;
        }
        if (target(&snapshot)) {
            return DCC_OK;
        }
        if (terminal(&snapshot)) {
            return DCC_ERR_STATE;
        }
        if (timeout_ms != 0U && llam_now_ns() - start_ns >= timeout_ns) {
            return DCC_ERR_TIMEOUT;
        }
        (void)llam_sleep_ns(UINT64_C(1000000));
    }
}

dcc_status_t dcc_interaction_server_wait_until_ready(
    dcc_interaction_server_t *server,
    uint32_t timeout_ms,
    dcc_interaction_server_health_snapshot_t *out
) {
    return dcc_interaction_server_wait_until(
        server,
        timeout_ms,
        out,
        dcc_interaction_server_wait_ready_target,
        dcc_interaction_server_wait_ready_terminal
    );
}

dcc_status_t dcc_interaction_server_wait_until_drained(
    dcc_interaction_server_t *server,
    uint32_t timeout_ms,
    dcc_interaction_server_health_snapshot_t *out
) {
    return dcc_interaction_server_wait_until(
        server,
        timeout_ms,
        out,
        dcc_interaction_server_wait_drained_target,
        dcc_interaction_server_wait_drained_terminal
    );
}
