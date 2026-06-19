#include "internal/client/dcc_client_state_internal.h"
#include "internal/dcc_core_internal.h"
#include "internal/rest/dcc_rest_async_status_internal.h"
#include "internal/rest/dcc_rest_firewall_internal.h"
#include "internal/rest/dcc_rest_rate_limit_internal.h"
#include "internal/rest/dcc_rest_runtime_internal.h"

#include <stddef.h>
#include <string.h>

#define DCC_REST_FIREWALL_SNAPSHOT_BASE_SIZE \
    (offsetof(dcc_rest_firewall_snapshot_t, attached) + sizeof(((dcc_rest_firewall_snapshot_t *)0)->attached))

dcc_status_t dcc_rest_firewall_attach(
    dcc_client_t *client,
    const dcc_rest_firewall_options_t *options
) {
    if (client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (options != NULL && dcc_rest_firewall_options_validate(options) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_firewall_state_t *state = dcc_rest_firewall_state_new(options, 1U);
    if (state == NULL) {
        return DCC_ERR_NOMEM;
    }

    dcc_rest_lock(client);
    dcc_rest_firewall_state_t *old = client->rest_firewall;
    client->rest_firewall = state;
    dcc_rest_unlock(client);

    dcc_rest_firewall_state_free(old);
    return DCC_OK;
}

void dcc_rest_firewall_detach(dcc_client_t *client) {
    if (client == NULL) {
        return;
    }

    dcc_rest_lock(client);
    dcc_rest_firewall_state_t *old = client->rest_firewall;
    client->rest_firewall = NULL;
    dcc_rest_unlock(client);

    dcc_rest_firewall_state_free(old);
}

dcc_status_t dcc_rest_firewall_snapshot(
    dcc_client_t *client,
    dcc_rest_firewall_snapshot_t *snapshot
) {
    if (client == NULL || snapshot == NULL || snapshot->size < DCC_REST_FIREWALL_SNAPSHOT_BASE_SIZE) {
        return DCC_ERR_INVALID_ARG;
    }

    size_t out_size = snapshot->size;
    uint64_t now_ms = dcc_rest_now_ms();

    dcc_rest_firewall_snapshot_t full;
    memset(&full, 0, sizeof(full));
    dcc_rest_lock(client);
    if (client->rest_firewall != NULL) {
        dcc_rest_firewall_state_normalize_window(client->rest_firewall, now_ms);
        dcc_rest_firewall_snapshot_from_state(client->rest_firewall, now_ms, &full);
    } else {
        dcc_rest_firewall_snapshot_from_state(NULL, now_ms, &full);
    }
    full.rest.size = sizeof(full.rest);
    dcc_rest_async_status_snapshot_locked(client, now_ms, &full.rest);
    dcc_rest_unlock(client);

    size_t copy_size = out_size < sizeof(full) ? out_size : sizeof(full);
    memcpy(snapshot, &full, copy_size);
    return DCC_OK;
}

dcc_status_t dcc_rest_firewall_before_request(
    dcc_client_t *client,
    const char *method,
    const char *path
) {
    if (client == NULL || method == NULL || path == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_firewall_result_t result;
    memset(&result, 0, sizeof(result));

    dcc_rest_lock(client);
    dcc_rest_firewall_state_t *state = client->rest_firewall;
    if (state == NULL) {
        dcc_rest_unlock(client);
        return DCC_OK;
    }
    dcc_rest_firewall_request_t request = {
        .size = sizeof(request),
        .method = method,
        .path = path,
        .now_ms = dcc_rest_now_ms(),
        .critical = 0U,
    };
    dcc_status_t status = dcc_rest_firewall_state_check(state, &request, &result);
    dcc_rest_unlock(client);
    if (status != DCC_OK) {
        return status;
    }

    if (result.decision == DCC_REST_FIREWALL_REJECT) {
        dcc_set_error(client, "REST firewall rejected noncritical request");
        return DCC_ERR_STATE;
    }
    if (result.decision == DCC_REST_FIREWALL_DELAY && result.delay_ms != 0) {
        dcc_rest_sleep_ms(result.delay_ms);
    }
    return DCC_OK;
}

void dcc_rest_firewall_after_response(dcc_client_t *client, uint16_t status) {
    if (client == NULL) {
        return;
    }
    dcc_rest_lock(client);
    dcc_rest_firewall_state_t *state = client->rest_firewall;
    if (state != NULL) {
        dcc_rest_firewall_state_record_response(state, status, dcc_rest_now_ms());
    }
    dcc_rest_unlock(client);
}
