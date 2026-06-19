#include "internal/interactions/dcc_interaction_server_internal.h"

#include <stdint.h>
#include <string.h>

static void dcc_interaction_health_route_callback(
    dcc_interaction_server_t *server,
    dcc_interaction_request_t *request,
    void *user_data
) {
    (void)user_data;

    dcc_interaction_server_health_snapshot_t health = {
        .size = sizeof(health),
    };
    char body[2048];
    if (dcc_interaction_server_health_snapshot(server, &health) != DCC_OK ||
        dcc_interaction_server_health_snapshot_json(&health, body, sizeof(body), NULL) != DCC_OK) {
        (void)dcc_interaction_request_reply_text(request, 500, "health unavailable");
        return;
    }

    uint16_t status = health.ok != 0U ? 200U : 503U;
    (void)dcc_interaction_request_set_response(
        request,
        status,
        "application/json",
        body,
        strlen(body)
    );
}

dcc_status_t dcc_interaction_server_add_health_route(
    dcc_interaction_server_t *server,
    const char *path
) {
    if (server == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_interaction_route_t route = {
        .size = sizeof(route),
        .method = "GET",
        .path = path != NULL && path[0] != '\0' ? path : "/healthz",
        .flags = (uint32_t)DCC_INTERACTION_ROUTE_F_UNSIGNED,
        .callback = dcc_interaction_health_route_callback,
        .user_data = NULL,
    };
    return dcc_interaction_server_add_route(server, &route);
}
