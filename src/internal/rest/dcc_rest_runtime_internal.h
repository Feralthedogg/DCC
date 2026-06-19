#ifndef DCC_REST_RUNTIME_INTERNAL_H
#define DCC_REST_RUNTIME_INTERNAL_H

#include <dcc/dcc.h>

#include <stdint.h>
#if !defined(_WIN32)
#include <time.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

uint64_t dcc_rest_now_ms(void);
uint64_t dcc_rest_seconds_to_ms(double seconds);
uint64_t dcc_rest_add_ms(uint64_t base, uint64_t delta);
void dcc_rest_sleep_ms(uint64_t ms);
dcc_status_t dcc_rest_wait_status_from_errno(int err);
uint64_t dcc_rest_timeout_deadline_ns(uint32_t timeout_ms);
#if !defined(_WIN32)
dcc_status_t dcc_rest_status_from_pthread(int rc);
dcc_status_t dcc_rest_make_deadline_ms(uint64_t wait_ms, struct timespec *out);
#endif

#ifdef __cplusplus
}
#endif

#endif
