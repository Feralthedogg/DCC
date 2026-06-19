#include "internal/rest/dcc_rest_firewall_internal.h"

#include <string.h>

dcc_status_t dcc_rest_firewall_init(
    dcc_rest_firewall_t *firewall,
    const dcc_rest_firewall_options_t *options
) {
    if (firewall == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    memset(firewall, 0, sizeof(*firewall));
    firewall->size = sizeof(*firewall);
    firewall->state = dcc_rest_firewall_state_new(options, 0U);
    return firewall->state != NULL ? DCC_OK : DCC_ERR_NOMEM;
}

void dcc_rest_firewall_deinit(dcc_rest_firewall_t *firewall) {
    if (firewall == NULL) {
        return;
    }
    dcc_rest_firewall_state_free((dcc_rest_firewall_state_t *)firewall->state);
    firewall->state = NULL;
    firewall->size = sizeof(*firewall);
}

dcc_status_t dcc_rest_firewall_check(
    dcc_rest_firewall_t *firewall,
    const dcc_rest_firewall_request_t *request,
    dcc_rest_firewall_result_t *result
) {
    if (firewall == NULL || firewall->state == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_firewall_state_check(
        (dcc_rest_firewall_state_t *)firewall->state,
        request,
        result
    );
}

dcc_status_t dcc_rest_firewall_record_response(
    dcc_rest_firewall_t *firewall,
    uint16_t status,
    uint64_t now_ms
) {
    if (firewall == NULL || firewall->state == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_rest_firewall_state_record_response(
        (dcc_rest_firewall_state_t *)firewall->state,
        status,
        now_ms
    );
    return DCC_OK;
}

dcc_status_t dcc_rest_firewall_state_snapshot(
    dcc_rest_firewall_t *firewall,
    uint64_t now_ms,
    dcc_rest_firewall_snapshot_t *snapshot
) {
    if (firewall == NULL || firewall->state == NULL || snapshot == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_rest_firewall_state_t *state = (dcc_rest_firewall_state_t *)firewall->state;
    dcc_rest_firewall_state_normalize_window(state, now_ms);
    dcc_rest_firewall_snapshot_from_state(state, now_ms, snapshot);
    return DCC_OK;
}
