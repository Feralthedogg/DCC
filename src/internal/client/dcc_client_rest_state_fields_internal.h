#ifndef DCC_CLIENT_REST_STATE_FIELDS_INTERNAL_H
#define DCC_CLIENT_REST_STATE_FIELDS_INTERNAL_H

#include "internal/rest/dcc_rest_intercept_internal.h"
#include "internal/rest/dcc_rest_state_internal.h"

#include <dcc/client.h>

#include <stdatomic.h>
#include <stdint.h>
#if !defined(_WIN32)
#include <pthread.h>
#endif

struct dcc_rest_firewall_state;

#if !defined(_WIN32)
#define DCC_CLIENT_REST_WAIT_STATE_FIELDS \
    pthread_mutex_t rest_wait_mutex; \
    pthread_cond_t rest_wait_cond; \
    uint8_t rest_wait_initialized;
#else
#define DCC_CLIENT_REST_WAIT_STATE_FIELDS
#endif

#define DCC_CLIENT_REST_STATE_FIELDS \
    atomic_flag rest_lock; \
    atomic_flag rest_error_observer_lock; \
    dcc_client_error_fn rest_error_observer; \
    void *rest_error_observer_user_data; \
    dcc_client_error_fn rest_app_error_sink; \
    void *rest_app_error_sink_user_data; \
    atomic_uint rest_app_error_sink_in_flight; \
    atomic_uint rest_terminal_in_flight; \
    uint32_t rest_operations_in_flight; \
    uint8_t rest_initialized; \
    uint8_t rest_admission_closed; \
    uint8_t rest_terminal_closed; \
    uint8_t rest_quiesced; \
    dcc_rest_intercept_fn rest_intercept; \
    void *rest_intercept_user_data; \
    struct dcc_rest_firewall_state *rest_firewall; \
    uint64_t rest_global_reset_at_ms; \
    dcc_rest_bucket_t rest_buckets[DCC_REST_BUCKET_CAP]; \
    dcc_rest_async_request_t *rest_async_heads[DCC_REST_PRIORITY_LEVELS]; \
    dcc_rest_async_request_t *rest_async_tails[DCC_REST_PRIORITY_LEVELS]; \
    dcc_rest_async_request_t *rest_async_active_head; \
    char rest_async_active_routes[DCC_REST_BUCKET_CAP][DCC_REST_ROUTE_KEY_CAP]; \
    uint32_t rest_async_active_route_count; \
    uint32_t rest_async_active; \
    uint8_t rest_test_fail_next_worker_spawn; \
    DCC_CLIENT_REST_WAIT_STATE_FIELDS

#endif
