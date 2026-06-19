#ifndef DCC_INTERACTION_ROUTE_INTERNAL_H
#define DCC_INTERACTION_ROUTE_INTERNAL_H

#include <dcc/interaction_server.h>

#include <stddef.h>
#include <stdint.h>

typedef struct dcc_interaction_route_entry {
    char *method;
    char *path;
    uint32_t flags;
    dcc_interaction_server_cb callback;
    void *user_data;
} dcc_interaction_route_entry_t;

#ifdef __cplusplus
extern "C" {
#endif

const dcc_interaction_route_entry_t *dcc_interaction_find_route(
    const dcc_interaction_server_t *server,
    const dcc_interaction_request_t *request
);
const dcc_interaction_route_entry_t *dcc_interaction_find_prefix_route(
    const dcc_interaction_server_t *server,
    const dcc_interaction_request_t *request
);
int dcc_interaction_has_signed_route_for_method(
    const dcc_interaction_server_t *server,
    const char *method
);
void dcc_interaction_route_entry_free(dcc_interaction_route_entry_t *route);
dcc_status_t dcc_interaction_routes_reserve(dcc_interaction_server_t *server, size_t need);
void dcc_interaction_server_routes_free(dcc_interaction_server_t *server);

#ifdef __cplusplus
}
#endif

#endif
