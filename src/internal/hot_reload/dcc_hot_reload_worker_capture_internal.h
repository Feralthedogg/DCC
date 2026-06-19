#ifndef DCC_HOT_RELOAD_WORKER_CAPTURE_INTERNAL_H
#define DCC_HOT_RELOAD_WORKER_CAPTURE_INTERNAL_H

#include "internal/hot_reload/dcc_hot_reload_worker_types_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

void dcc_hot_reload_worker_capture_start(
    dcc_client_t *client,
    dcc_hot_reload_worker_capture_t *capture
);
void dcc_hot_reload_worker_capture_stop(dcc_client_t *client);
void dcc_hot_reload_worker_capture_deinit(dcc_hot_reload_worker_capture_t *capture);
dcc_status_t dcc_hot_reload_worker_capture_push(
    dcc_hot_reload_worker_capture_t *capture,
    const char *method,
    const char *path,
    const void *body,
    size_t body_len,
    const char *content_type
);
int dcc_hot_reload_worker_capture_send_result(
    dcc_hot_reload_worker_capture_t *capture,
    dcc_status_t status
);

#ifdef __cplusplus
}
#endif

#endif
