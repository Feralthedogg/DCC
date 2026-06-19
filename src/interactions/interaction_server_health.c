#include "internal/interactions/dcc_interaction_server_internal.h"

#include <string.h>

static dcc_interaction_server_health_t dcc_interaction_health_classify(
    const dcc_interaction_server_state_t *state,
    const dcc_interaction_server_stats_t *stats
) {
    if (state == NULL || state->size < sizeof(*state) ||
        stats == NULL || stats->size < sizeof(*stats)) {
        return DCC_INTERACTION_SERVER_HEALTH_UNKNOWN;
    }
    if (state->draining != 0U || (state->stopping != 0U && state->active_requests != 0U)) {
        return DCC_INTERACTION_SERVER_HEALTH_DRAINING;
    }
    if (state->started == 0U || (state->stopping != 0U && state->active_requests == 0U)) {
        return DCC_INTERACTION_SERVER_HEALTH_STOPPED;
    }
    if (state->listening == 0U ||
        stats->spawn_errors != 0U ||
        stats->write_errors != 0U ||
        stats->response_5xx != 0U) {
        return DCC_INTERACTION_SERVER_HEALTH_DEGRADED;
    }
    return DCC_INTERACTION_SERVER_HEALTH_READY;
}

static const char *dcc_interaction_health_reason(
    dcc_interaction_server_health_t health,
    const dcc_interaction_server_state_t *state,
    const dcc_interaction_server_stats_t *stats
) {
    if (state == NULL || stats == NULL) {
        return "health unavailable";
    }
    if (stats->spawn_errors != 0U) {
        return "request spawn failures observed";
    }
    if (stats->write_errors != 0U) {
        return "response write failures observed";
    }
    if (stats->response_5xx != 0U) {
        return "5xx responses observed";
    }
    switch (health) {
        case DCC_INTERACTION_SERVER_HEALTH_READY:
            return "server is ready";
        case DCC_INTERACTION_SERVER_HEALTH_DEGRADED:
            return "server is not accepting while started";
        case DCC_INTERACTION_SERVER_HEALTH_DRAINING:
            return "server is draining active requests";
        case DCC_INTERACTION_SERVER_HEALTH_STOPPED:
            return state->stopping != 0U ? "server is stopped after drain" : "server is stopped";
        case DCC_INTERACTION_SERVER_HEALTH_UNKNOWN:
        default:
            return "health unavailable";
    }
}

dcc_status_t dcc_interaction_server_health_snapshot(
    const dcc_interaction_server_t *server,
    dcc_interaction_server_health_snapshot_t *out
) {
    if (server == NULL || out == NULL || out->size < sizeof(*out)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_interaction_server_health_snapshot_t snapshot;
    memset(&snapshot, 0, sizeof(snapshot));
    snapshot.size = sizeof(snapshot);
    snapshot.state.size = sizeof(snapshot.state);
    snapshot.stats.size = sizeof(snapshot.stats);

    dcc_interaction_server_fill_state(server, &snapshot.state);
    dcc_status_t status = dcc_interaction_server_stats(server, &snapshot.stats);
    if (status != DCC_OK) {
        return status;
    }

    snapshot.health = dcc_interaction_health_classify(&snapshot.state, &snapshot.stats);
    snapshot.ready = snapshot.state.started != 0U &&
        snapshot.state.listening != 0U &&
        snapshot.state.stopping == 0U
        ? 1U
        : 0U;
    snapshot.accepting = snapshot.ready;
    snapshot.drain_complete = snapshot.state.stopping != 0U &&
        snapshot.state.active_requests == 0U
        ? 1U
        : 0U;
    snapshot.ok = snapshot.health == DCC_INTERACTION_SERVER_HEALTH_READY ? 1U : 0U;
    dcc_interaction_copy_text(
        snapshot.reason,
        sizeof(snapshot.reason),
        dcc_interaction_health_reason(snapshot.health, &snapshot.state, &snapshot.stats)
    );
    *out = snapshot;
    return DCC_OK;
}
