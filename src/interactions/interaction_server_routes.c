#include "internal/interactions/dcc_interaction_server_internal.h"

#include <string.h>

dcc_status_t dcc_interaction_server_add_route(
    dcc_interaction_server_t *server,
    const dcc_interaction_route_t *route
) {
    if (server == NULL || route == NULL || route->size < sizeof(*route) ||
        route->method == NULL || route->path == NULL || route->callback == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (atomic_load_explicit(&server->started, memory_order_acquire)) {
        return DCC_ERR_STATE;
    }
    uint32_t allowed_flags = (uint32_t)DCC_INTERACTION_ROUTE_F_UNSIGNED |
        (uint32_t)DCC_INTERACTION_ROUTE_F_PREFIX;
    if ((route->flags & ~allowed_flags) != 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    size_t method_len = strlen(route->method);
    size_t path_len = strlen(route->path);
    if (method_len == 0U || method_len >= 16U ||
        path_len == 0U || path_len >= 256U || route->path[0] != '/') {
        return DCC_ERR_INVALID_ARG;
    }
    if (server->route_count == SIZE_MAX) {
        return DCC_ERR_NOMEM;
    }

    dcc_status_t status = dcc_interaction_routes_reserve(server, server->route_count + 1U);
    if (status != DCC_OK) {
        return status;
    }
    dcc_interaction_route_entry_t *entry = &server->routes[server->route_count];
    entry->method = dcc_strdup(route->method);
    entry->path = dcc_strdup(route->path);
    if (entry->method == NULL || entry->path == NULL) {
        dcc_interaction_route_entry_free(entry);
        return DCC_ERR_NOMEM;
    }
    entry->flags = route->flags;
    entry->callback = route->callback;
    entry->user_data = route->user_data;
    server->route_count++;
    return DCC_OK;
}
dcc_status_t dcc_interaction_server_clear_routes(dcc_interaction_server_t *server) {
    if (server == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (atomic_load_explicit(&server->started, memory_order_acquire)) {
        return DCC_ERR_STATE;
    }
    dcc_interaction_server_routes_free(server);
    return DCC_OK;
}
