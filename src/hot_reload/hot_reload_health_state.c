#include "internal/hot_reload/dcc_hot_reload_internal.h"

dcc_hot_reload_health_t dcc_hot_reload_classify_health(
    uint8_t loaded,
    uint8_t running,
    uint8_t reloading,
    dcc_status_t last_status
) {
    if (last_status != DCC_OK) {
        return DCC_HOT_RELOAD_HEALTH_DEGRADED;
    }
    if (reloading != 0U) {
        return DCC_HOT_RELOAD_HEALTH_RELOADING;
    }
    if (loaded != 0U && running != 0U) {
        return DCC_HOT_RELOAD_HEALTH_WATCHING;
    }
    return DCC_HOT_RELOAD_HEALTH_STOPPED;
}

const char *dcc_hot_reload_health_reason(
    dcc_hot_reload_health_t health,
    uint8_t loaded,
    uint8_t pending_reload
) {
    if (pending_reload != 0U) {
        return "module file changed and is waiting for settle interval";
    }
    switch (health) {
        case DCC_HOT_RELOAD_HEALTH_WATCHING:
            return "hot reload watcher is running";
        case DCC_HOT_RELOAD_HEALTH_RELOADING:
            return "hot reload is loading a new module generation";
        case DCC_HOT_RELOAD_HEALTH_DEGRADED:
            return loaded != 0U
                ? "last hot reload failed and previous generation is still loaded"
                : "hot reload module is not loaded";
        case DCC_HOT_RELOAD_HEALTH_STOPPED:
            return loaded != 0U ? "hot reload watcher is stopped" : "hot reload module is not loaded";
        case DCC_HOT_RELOAD_HEALTH_UNKNOWN:
        default:
            return "hot reload health unavailable";
    }
}
