#ifndef DCC_GATEWAY_TIMING_INTERNAL_H
#define DCC_GATEWAY_TIMING_INTERNAL_H

#include <dcc/dcc.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint64_t dcc_gateway_now_ms(void);
uint32_t dcc_gateway_now_jitter(uint32_t max_ms, uint64_t salt);
dcc_status_t dcc_gateway_wait_ms(dcc_client_t *client, uint64_t delay_ms);

#ifdef __cplusplus
}
#endif

#endif
