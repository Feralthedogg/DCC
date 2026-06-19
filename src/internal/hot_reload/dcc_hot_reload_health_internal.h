#ifndef DCC_HOT_RELOAD_HEALTH_INTERNAL_H
#define DCC_HOT_RELOAD_HEALTH_INTERNAL_H

#include <dcc/dcc.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_hot_reload_health_t dcc_hot_reload_classify_health(
    uint8_t loaded,
    uint8_t running,
    uint8_t reloading,
    dcc_status_t last_status
);

const char *dcc_hot_reload_health_reason(
    dcc_hot_reload_health_t health,
    uint8_t loaded,
    uint8_t pending_reload
);

#ifdef __cplusplus
}
#endif

#endif
