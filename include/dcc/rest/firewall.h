#ifndef DCC_REST_FIREWALL_H
#define DCC_REST_FIREWALL_H

#include <dcc/error.h>
#include <dcc/export.h>
#include <dcc/rest/base.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DCC_REST_FIREWALL_DEFAULT_INVALID_REQUEST_WINDOW_MS 600000ULL
#define DCC_REST_FIREWALL_DEFAULT_INVALID_REQUEST_SOFT_LIMIT 8000U
#define DCC_REST_FIREWALL_DEFAULT_INVALID_REQUEST_HARD_LIMIT 9500U
#define DCC_REST_FIREWALL_DEFAULT_SOFT_LIMIT_DELAY_MS 1000U

typedef enum dcc_rest_firewall_hard_limit_action {
    DCC_REST_FIREWALL_HARD_LIMIT_REJECT_NONCRITICAL = 0,
    DCC_REST_FIREWALL_HARD_LIMIT_REJECT_ALL,
    DCC_REST_FIREWALL_HARD_LIMIT_DELAY
} dcc_rest_firewall_hard_limit_action_t;

typedef enum dcc_rest_firewall_decision {
    DCC_REST_FIREWALL_ALLOW = 0,
    DCC_REST_FIREWALL_DELAY,
    DCC_REST_FIREWALL_REJECT
} dcc_rest_firewall_decision_t;

typedef enum dcc_rest_firewall_reason {
    DCC_REST_FIREWALL_REASON_NONE = 0,
    DCC_REST_FIREWALL_REASON_SOFT_INVALID_REQUEST_LIMIT,
    DCC_REST_FIREWALL_REASON_HARD_INVALID_REQUEST_LIMIT
} dcc_rest_firewall_reason_t;

typedef struct dcc_rest_firewall_options {
    size_t size;
    uint32_t invalid_request_soft_limit;
    uint32_t invalid_request_hard_limit;
    uint64_t invalid_request_window_ms;
    uint32_t soft_limit_delay_ms;
    dcc_rest_firewall_hard_limit_action_t on_hard_limit;
} dcc_rest_firewall_options_t;

typedef struct dcc_rest_firewall_request {
    size_t size;
    const char *method;
    const char *path;
    uint64_t now_ms;
    uint8_t critical;
} dcc_rest_firewall_request_t;

typedef struct dcc_rest_firewall_result {
    size_t size;
    dcc_rest_firewall_decision_t decision;
    dcc_rest_firewall_reason_t reason;
    uint64_t delay_ms;
    uint8_t critical;
} dcc_rest_firewall_result_t;

typedef struct dcc_rest_firewall_snapshot {
    size_t size;
    dcc_rest_firewall_options_t options;
    dcc_rest_async_status_t rest;
    uint64_t window_started_at_ms;
    uint64_t window_ends_at_ms;
    size_t invalid_request_count;
    uint64_t total_requests;
    uint64_t total_invalid_requests;
    uint64_t total_delayed_requests;
    uint64_t total_rejected_requests;
    uint8_t soft_limit_exceeded;
    uint8_t hard_limit_exceeded;
    uint8_t attached;
} dcc_rest_firewall_snapshot_t;

typedef struct dcc_rest_firewall {
    size_t size;
    void *state;
} dcc_rest_firewall_t;

DCC_API void dcc_rest_firewall_options_init(dcc_rest_firewall_options_t *options);
DCC_API dcc_status_t dcc_rest_firewall_options_validate(
    const dcc_rest_firewall_options_t *options
);

DCC_API dcc_status_t dcc_rest_firewall_init(
    dcc_rest_firewall_t *firewall,
    const dcc_rest_firewall_options_t *options
);
DCC_API void dcc_rest_firewall_deinit(dcc_rest_firewall_t *firewall);
DCC_API dcc_status_t dcc_rest_firewall_check(
    dcc_rest_firewall_t *firewall,
    const dcc_rest_firewall_request_t *request,
    dcc_rest_firewall_result_t *result
);
DCC_API dcc_status_t dcc_rest_firewall_record_response(
    dcc_rest_firewall_t *firewall,
    uint16_t status,
    uint64_t now_ms
);
DCC_API dcc_status_t dcc_rest_firewall_state_snapshot(
    dcc_rest_firewall_t *firewall,
    uint64_t now_ms,
    dcc_rest_firewall_snapshot_t *snapshot
);

DCC_API dcc_status_t dcc_rest_firewall_attach(
    dcc_client_t *client,
    const dcc_rest_firewall_options_t *options
);
DCC_API void dcc_rest_firewall_detach(dcc_client_t *client);
DCC_API dcc_status_t dcc_rest_firewall_snapshot(
    dcc_client_t *client,
    dcc_rest_firewall_snapshot_t *snapshot
);
DCC_API dcc_status_t dcc_rest_firewall_snapshot_json(
    const dcc_rest_firewall_snapshot_t *snapshot,
    char *buf,
    size_t len,
    size_t *out_len
);

DCC_API uint8_t dcc_rest_firewall_status_is_invalid(uint16_t status);
DCC_API uint8_t dcc_rest_firewall_request_is_critical(
    const char *method,
    const char *path
);
DCC_API const char *dcc_rest_firewall_decision_string(dcc_rest_firewall_decision_t decision);
DCC_API const char *dcc_rest_firewall_reason_string(dcc_rest_firewall_reason_t reason);
DCC_API const char *dcc_rest_firewall_hard_limit_action_string(
    dcc_rest_firewall_hard_limit_action_t action
);

#ifdef __cplusplus
}
#endif

#endif
