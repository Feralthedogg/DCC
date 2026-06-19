#include "internal/interactions/dcc_interaction_server_internal.h"

#include <stdlib.h>
#include <string.h>

void dcc_interaction_route_entry_free(dcc_interaction_route_entry_t *route) {
    if (route == NULL) {
        return;
    }
    free(route->method);
    free(route->path);
    memset(route, 0, sizeof(*route));
}

void dcc_interaction_server_routes_free(dcc_interaction_server_t *server) {
    if (server == NULL || server->routes == NULL) {
        return;
    }
    for (size_t i = 0; i < server->route_count; ++i) {
        dcc_interaction_route_entry_free(&server->routes[i]);
    }
    free(server->routes);
    server->routes = NULL;
    server->route_count = 0;
    server->route_cap = 0;
}

dcc_status_t dcc_interaction_routes_reserve(dcc_interaction_server_t *server, size_t need) {
    if (need <= server->route_cap) {
        return DCC_OK;
    }
    size_t next_cap = server->route_cap != 0U ? server->route_cap : 4U;
    while (next_cap < need) {
        if (next_cap > SIZE_MAX / 2U) {
            return DCC_ERR_NOMEM;
        }
        next_cap *= 2U;
    }
    if (next_cap > SIZE_MAX / sizeof(*server->routes)) {
        return DCC_ERR_NOMEM;
    }
    dcc_interaction_route_entry_t *routes = (dcc_interaction_route_entry_t *)realloc(
        server->routes,
        next_cap * sizeof(*server->routes)
    );
    if (routes == NULL) {
        return DCC_ERR_NOMEM;
    }
    memset(routes + server->route_cap, 0, (next_cap - server->route_cap) * sizeof(*routes));
    server->routes = routes;
    server->route_cap = next_cap;
    return DCC_OK;
}
