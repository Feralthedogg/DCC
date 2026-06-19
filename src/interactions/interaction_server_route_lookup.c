#include "internal/interactions/dcc_interaction_server_internal.h"

#include <string.h>

static int dcc_interaction_path_prefix_eq(const char *path, const char *prefix) {
    if (path == NULL || prefix == NULL) {
        return 0;
    }
    size_t prefix_len = strlen(prefix);
    return strncmp(path, prefix, prefix_len) == 0 ? 1 : 0;
}

const dcc_interaction_route_entry_t *dcc_interaction_find_route(
    const dcc_interaction_server_t *server,
    const dcc_interaction_request_t *request
) {
    if (server == NULL || request == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < server->route_count; ++i) {
        const dcc_interaction_route_entry_t *route = &server->routes[i];
        if (!dcc_interaction_ascii_eq(route->method, request->method)) {
            continue;
        }
        if ((route->flags & (uint32_t)DCC_INTERACTION_ROUTE_F_PREFIX) == 0U &&
            strcmp(route->path, request->path) == 0) {
            return route;
        }
    }
    return NULL;
}

const dcc_interaction_route_entry_t *dcc_interaction_find_prefix_route(
    const dcc_interaction_server_t *server,
    const dcc_interaction_request_t *request
) {
    if (server == NULL || request == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < server->route_count; ++i) {
        const dcc_interaction_route_entry_t *route = &server->routes[i];
        if (!dcc_interaction_ascii_eq(route->method, request->method) ||
            (route->flags & (uint32_t)DCC_INTERACTION_ROUTE_F_PREFIX) == 0U) {
            continue;
        }
        if (dcc_interaction_path_prefix_eq(request->path, route->path)) {
            return route;
        }
    }
    return NULL;
}

int dcc_interaction_has_signed_route_for_method(
    const dcc_interaction_server_t *server,
    const char *method
) {
    if (server == NULL || method == NULL) {
        return 0;
    }
    for (size_t i = 0; i < server->route_count; ++i) {
        const dcc_interaction_route_entry_t *route = &server->routes[i];
        if ((route->flags & (uint32_t)DCC_INTERACTION_ROUTE_F_UNSIGNED) == 0U &&
            dcc_interaction_ascii_eq(route->method, method)) {
            return 1;
        }
    }
    return 0;
}
