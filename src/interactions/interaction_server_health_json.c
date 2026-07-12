#include "internal/interactions/dcc_interaction_server_internal.h"
#include "internal/interactions/dcc_interaction_health_json_internal.h"

#include <stdio.h>
#include <stddef.h>

const char *dcc_interaction_server_health_string(dcc_interaction_server_health_t health) {
    switch (health) {
        case DCC_INTERACTION_SERVER_HEALTH_READY:
            return "ready";
        case DCC_INTERACTION_SERVER_HEALTH_DEGRADED:
            return "degraded";
        case DCC_INTERACTION_SERVER_HEALTH_DRAINING:
            return "draining";
        case DCC_INTERACTION_SERVER_HEALTH_STOPPED:
            return "stopped";
        case DCC_INTERACTION_SERVER_HEALTH_UNKNOWN:
        default:
            return "unknown";
    }
}

dcc_status_t dcc_interaction_server_health_snapshot_json(
    const dcc_interaction_server_health_snapshot_t *snapshot,
    char *out,
    size_t out_size,
    size_t *out_len
) {
    if (snapshot == NULL || snapshot->size < offsetof(dcc_interaction_server_health_snapshot_t, protection) ||
        out == NULL || out_size == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_interaction_server_protection_stats_t protection = {0};
    if (snapshot->size >= offsetof(dcc_interaction_server_health_snapshot_t, protection) +
            sizeof(snapshot->protection)) {
        protection = snapshot->protection;
    }

    char reason_json[1024];
    dcc_status_t status = dcc_interaction_json_escape_string(
        snapshot->reason,
        reason_json,
        sizeof(reason_json)
    );
    if (status != DCC_OK) {
        return status;
    }

    int len = snprintf(
        out,
        out_size,
        "{\"ok\":%s,\"ready\":%s,\"health\":\"%s\",\"accepting\":%s,"
        "\"draining\":%s,\"drain_complete\":%s,\"port\":%u,\"routes\":%zu,"
        "\"accepted\":%llu,\"active\":%llu,\"completed\":%llu,"
        "\"read_errors\":%llu,\"write_errors\":%llu,\"spawn_errors\":%llu,"
        "\"4xx\":%llu,\"5xx\":%llu,\"overloaded\":%llu,\"replayed\":%llu,"
        "\"deadline_exceeded\":%llu,\"reason\":%s}",
        snapshot->ok != 0U ? "true" : "false",
        snapshot->ready != 0U ? "true" : "false",
        dcc_interaction_server_health_string(snapshot->health),
        snapshot->accepting != 0U ? "true" : "false",
        snapshot->state.draining != 0U ? "true" : "false",
        snapshot->drain_complete != 0U ? "true" : "false",
        (unsigned)snapshot->state.bound_port,
        snapshot->state.route_count,
        (unsigned long long)snapshot->stats.accepted_connections,
        (unsigned long long)snapshot->stats.active_requests,
        (unsigned long long)snapshot->stats.completed_requests,
        (unsigned long long)snapshot->stats.read_errors,
        (unsigned long long)snapshot->stats.write_errors,
        (unsigned long long)snapshot->stats.spawn_errors,
        (unsigned long long)snapshot->stats.response_4xx,
        (unsigned long long)snapshot->stats.response_5xx,
        (unsigned long long)protection.overloaded_responses,
        (unsigned long long)protection.replayed_requests,
        (unsigned long long)protection.deadline_exceeded_requests,
        reason_json
    );
    if (len < 0) {
        return DCC_ERR_RUNTIME;
    }
    if (out_len != NULL) {
        *out_len = (size_t)len;
    }
    return (size_t)len < out_size ? DCC_OK : DCC_ERR_NOMEM;
}
