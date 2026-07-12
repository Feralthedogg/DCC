#ifndef DCC_GATEWAY_RUNTIME_INTERNAL_H
#define DCC_GATEWAY_RUNTIME_INTERNAL_H

#include "internal/gateway/dcc_gateway_session_internal.h"

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_gateway_run_once(
    dcc_client_t *client,
    const char *url,
    bool resume,
    dcc_gateway_next_t *next
);
dcc_status_t dcc_gateway_run(dcc_client_t *client);
char *dcc_gateway_attempt_url(dcc_client_t *client, bool resume);
uint32_t dcc_gateway_backoff_ms(unsigned attempt);
uint32_t dcc_gateway_backoff_jitter_ms(unsigned attempt, uint64_t salt);
void dcc_gateway_task(void *arg);

#ifdef __cplusplus
}
#endif

#endif
