#include "internal/rest/dcc_rest_firewall_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_firewall_create(
    const dcc_rest_firewall_options_t *options,
    dcc_rest_firewall_t **out_firewall
) {
    if (out_firewall == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_firewall = NULL;
    dcc_rest_firewall_t *firewall = calloc(1U, sizeof(*firewall));
    if (firewall == NULL) {
        return DCC_ERR_NOMEM;
    }
    firewall->state = dcc_rest_firewall_state_new(options, 0U);
    if (firewall->state == NULL) {
        free(firewall);
        return DCC_ERR_NOMEM;
    }
    *out_firewall = firewall;
    return DCC_OK;
}

void dcc_rest_firewall_destroy(dcc_rest_firewall_t *firewall) {
    if (firewall == NULL) {
        return;
    }
    dcc_rest_firewall_state_free((dcc_rest_firewall_state_t *)firewall->state);
    free(firewall);
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
