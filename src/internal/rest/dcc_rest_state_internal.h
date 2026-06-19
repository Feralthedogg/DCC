#ifndef DCC_REST_STATE_INTERNAL_H
#define DCC_REST_STATE_INTERNAL_H

#include <dcc/dcc.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DCC_REST_ROUTE_KEY_CAP 160U
#define DCC_REST_BUCKET_CAP 128U
#define DCC_REST_PRIORITY_LEVELS 3U

typedef struct dcc_rest_bucket {
    char route[DCC_REST_ROUTE_KEY_CAP];
    char bucket[DCC_REST_ROUTE_KEY_CAP];
    uint32_t remaining;
    uint64_t reset_at_ms;
    uint8_t active;
} dcc_rest_bucket_t;

typedef struct dcc_rest_async_request dcc_rest_async_request_t;

dcc_status_t dcc_rest_init(dcc_client_t *client);
void dcc_rest_deinit(dcc_client_t *client);

#ifdef __cplusplus
}
#endif

#endif
