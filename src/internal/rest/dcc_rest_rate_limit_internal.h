#ifndef DCC_REST_RATE_LIMIT_INTERNAL_H
#define DCC_REST_RATE_LIMIT_INTERNAL_H

#include "internal/client/dcc_client_state_internal.h"
#include "internal/http/dcc_http.h"

#include <dcc/dcc.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void dcc_rest_lock(dcc_client_t *client);
void dcc_rest_unlock(dcc_client_t *client);
const char *dcc_rest_response_header(
    const dcc_http_response_t *response,
    const char *name,
    size_t *out_len
);
double dcc_rest_parse_header_double(const char *value, size_t len);
int dcc_rest_parse_header_u32(const char *value, size_t len, uint32_t *out);
uint8_t dcc_rest_header_bool_true(const char *value, size_t len);
void dcc_rest_route_key(const char *method, const char *path, char *out, size_t out_size);
uint64_t dcc_rest_route_wait_ms_locked(
    dcc_client_t *client,
    const char *route,
    uint64_t now,
    int reset_expired
);
void dcc_rest_wait_for_route(dcc_client_t *client, const char *route);
void dcc_rest_update_rate_limit(
    dcc_client_t *client,
    const char *route,
    const dcc_http_response_t *response,
    double retry_after,
    uint8_t is_global
);

#ifdef __cplusplus
}
#endif

#endif
