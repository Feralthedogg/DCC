#include <dcc/hot_reload/sidecar.h>

#include <dcc/hot_reload/health.h>
#include <dcc/interaction_server/request.h>
#include <dcc/interaction_server/routes.h>

#include <string.h>

static void dcc_hot_reload_health_route_callback(
    dcc_interaction_server_t *server,
    dcc_interaction_request_t *request,
    void *user_data
) {
    (void)server;

    dcc_hot_reload_t *hot_reload = (dcc_hot_reload_t *)user_data;
    dcc_hot_reload_health_snapshot_t health = {
        .size = sizeof(health),
    };
    char body[4096];
    if (dcc_hot_reload_health_snapshot(hot_reload, &health) != DCC_OK ||
        dcc_hot_reload_health_snapshot_json(&health, body, sizeof(body), NULL) != DCC_OK) {
        (void)dcc_interaction_request_reply_text(request, 500, "hot reload health unavailable");
        return;
    }

    (void)dcc_interaction_request_set_response(
        request,
        health.ok != 0U ? 200U : 503U,
        "application/json",
        body,
        strlen(body)
    );
}

dcc_status_t dcc_hot_reload_add_health_route(
    dcc_interaction_server_t *server,
    const char *path,
    dcc_hot_reload_t *hot_reload
) {
    if (server == NULL || hot_reload == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_interaction_route_t route = {
        .size = sizeof(route),
        .method = "GET",
        .path = path != NULL && path[0] != '\0' ? path : "/hot-reloadz",
        .flags = (uint32_t)DCC_INTERACTION_ROUTE_F_UNSIGNED,
        .callback = dcc_hot_reload_health_route_callback,
        .user_data = hot_reload,
    };
    return dcc_interaction_server_add_route(server, &route);
}
