#ifndef DCC_CLIENT_GATEWAY_STATE_FIELDS_INTERNAL_H
#define DCC_CLIENT_GATEWAY_STATE_FIELDS_INTERNAL_H

#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>

#define DCC_CLIENT_GATEWAY_STATE_FIELDS \
    char *token; \
    char *gateway_url; \
    char *gateway_cached_url; \
    char *gateway_session_id; \
    char *gateway_resume_url; \
    uint64_t gateway_seq; \
    bool gateway_has_seq; \
    bool gateway_can_resume; \
    uint32_t gateway_recommended_shards; \
    uint32_t gateway_max_concurrency; \
    uint32_t gateway_remaining_identifies; \
    uint32_t gateway_session_total_identifies; \
    uint32_t gateway_identify_delay_ms; \
    uint64_t gateway_session_reset_after_ms; \
    uint64_t gateway_session_reset_at_ms; \
    atomic_bool gateway_task_running; \
    atomic_bool gateway_task_completed; \
    atomic_int gateway_last_status; \
    atomic_flag gateway_control_lock; \
    atomic_bool gateway_stop_requested; \
    atomic_bool gateway_reconnect_requested; \
    atomic_bool gateway_reconnect_resume; \
    struct dcc_ws *gateway_current_ws; \
    uint8_t gateway_stop_runtime_on_error

#endif
