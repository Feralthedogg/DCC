#ifndef DCC_REST_FIREWALL_INTERNAL_H
#define DCC_REST_FIREWALL_INTERNAL_H

#include <dcc/rest/firewall.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_rest_firewall_state {
    size_t size;
    dcc_rest_firewall_options_t options;
    uint64_t window_started_at_ms;
    size_t invalid_request_count;
    uint64_t total_requests;
    uint64_t total_invalid_requests;
    uint64_t total_delayed_requests;
    uint64_t total_rejected_requests;
    uint8_t window_active;
    uint8_t attached;
} dcc_rest_firewall_state_t;

dcc_rest_firewall_state_t *dcc_rest_firewall_state_new(
    const dcc_rest_firewall_options_t *options,
    uint8_t attached
);
void dcc_rest_firewall_state_free(dcc_rest_firewall_state_t *state);
void dcc_rest_firewall_state_normalize_window(
    dcc_rest_firewall_state_t *state,
    uint64_t now_ms
);
dcc_status_t dcc_rest_firewall_state_check(
    dcc_rest_firewall_state_t *state,
    const dcc_rest_firewall_request_t *request,
    dcc_rest_firewall_result_t *result
);
void dcc_rest_firewall_state_record_response(
    dcc_rest_firewall_state_t *state,
    uint16_t status,
    uint64_t now_ms
);
void dcc_rest_firewall_snapshot_from_state(
    const dcc_rest_firewall_state_t *state,
    uint64_t now_ms,
    dcc_rest_firewall_snapshot_t *snapshot
);
dcc_status_t dcc_rest_firewall_before_request(
    dcc_client_t *client,
    const char *method,
    const char *path
);
void dcc_rest_firewall_after_response(dcc_client_t *client, uint16_t status);

#ifdef __cplusplus
}
#endif

#endif
