#include "internal/rest/dcc_rest_async_queue_internal.h"
#include "internal/rest/dcc_rest_rate_limit_internal.h"

#include <stdio.h>
#include <string.h>

int dcc_rest_async_route_active_locked(const dcc_client_t *client, const char *route) {
    if (route == NULL || route[0] == '\0') {
        return 0;
    }
    for (uint32_t i = 0; i < client->rest_async_active_route_count; ++i) {
        if (strncmp(client->rest_async_active_routes[i], route, DCC_REST_ROUTE_KEY_CAP) == 0) {
            return 1;
        }
    }
    return 0;
}

uint64_t dcc_rest_async_request_wait_ms_locked(
    dcc_client_t *client,
    const dcc_rest_async_request_t *request,
    uint64_t now,
    int reset_expired
) {
    if (request == NULL) {
        return 0;
    }
    uint64_t wait_ms = dcc_rest_route_wait_ms_locked(client, request->route, now, reset_expired);
    if (request->not_before_ms > now) {
        uint64_t retry_wait = request->not_before_ms - now;
        if (retry_wait > wait_ms) {
            wait_ms = retry_wait;
        }
    }
    return wait_ms;
}

uint64_t dcc_rest_async_next_wake_ms_locked(dcc_client_t *client, uint64_t now) {
    uint64_t next_wait_ms = 0;
    for (uint32_t priority = 0; priority < DCC_REST_PRIORITY_LEVELS; ++priority) {
        const dcc_rest_async_request_t *request = client->rest_async_heads[priority];
        while (request != NULL) {
            if (!dcc_rest_async_route_active_locked(client, request->route)) {
                uint64_t wait_ms = dcc_rest_async_request_wait_ms_locked(client, request, now, 0);
                if (wait_ms != 0 && (next_wait_ms == 0 || wait_ms < next_wait_ms)) {
                    next_wait_ms = wait_ms;
                }
            }
            request = request->next;
        }
    }
    return next_wait_ms;
}

dcc_status_t dcc_rest_async_mark_route_active_locked(dcc_client_t *client, const char *route) {
    if (route == NULL || route[0] == '\0') {
        return DCC_OK;
    }
    if (dcc_rest_async_route_active_locked(client, route)) {
        return DCC_OK;
    }
    if (client->rest_async_active_route_count >= DCC_REST_BUCKET_CAP) {
        return DCC_ERR_STATE;
    }
    snprintf(
        client->rest_async_active_routes[client->rest_async_active_route_count],
        sizeof(client->rest_async_active_routes[client->rest_async_active_route_count]),
        "%s",
        route
    );
    client->rest_async_active_route_count++;
    return DCC_OK;
}

void dcc_rest_async_unmark_route_active_locked(dcc_client_t *client, const char *route) {
    if (route == NULL || route[0] == '\0') {
        return;
    }
    for (uint32_t i = 0; i < client->rest_async_active_route_count; ++i) {
        if (strncmp(client->rest_async_active_routes[i], route, DCC_REST_ROUTE_KEY_CAP) == 0) {
            uint32_t last = client->rest_async_active_route_count - 1U;
            if (i != last) {
                memcpy(
                    client->rest_async_active_routes[i],
                    client->rest_async_active_routes[last],
                    sizeof(client->rest_async_active_routes[i])
                );
            }
            memset(client->rest_async_active_routes[last], 0, sizeof(client->rest_async_active_routes[last]));
            client->rest_async_active_route_count--;
            return;
        }
    }
}
