#include <dcc/cluster/sidecar.h>

#include <dcc/cluster/health.h>
#include <dcc/interaction_server/request.h>
#include <dcc/interaction_server/routes.h>

#include <string.h>

static void dcc_cluster_status_route_callback(
    dcc_interaction_server_t *server,
    dcc_interaction_request_t *request,
    void *user_data
) {
    (void)server;

    dcc_cluster_t *cluster = (dcc_cluster_t *)user_data;
    char body[12288];
    dcc_status_t status = dcc_cluster_status_json(cluster, body, sizeof(body), NULL);
    if (status != DCC_OK) {
        (void)dcc_interaction_request_reply_text(request, 500, dcc_status_string(status));
        return;
    }

    (void)dcc_interaction_request_set_response(
        request,
        200,
        "application/json",
        body,
        strlen(body)
    );
}

dcc_status_t dcc_cluster_add_status_route(
    dcc_interaction_server_t *server,
    const char *path,
    dcc_cluster_t *cluster
) {
    if (server == NULL || cluster == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_interaction_route_t route = {
        .size = sizeof(route),
        .method = "GET",
        .path = path != NULL && path[0] != '\0' ? path : "/clusterz",
        .flags = (uint32_t)DCC_INTERACTION_ROUTE_F_UNSIGNED,
        .callback = dcc_cluster_status_route_callback,
        .user_data = cluster,
    };
    return dcc_interaction_server_add_route(server, &route);
}
