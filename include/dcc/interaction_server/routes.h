#ifndef DCC_INTERACTION_SERVER_ROUTES_H
#define DCC_INTERACTION_SERVER_ROUTES_H

#include <dcc/interaction_server/base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum dcc_interaction_route_flag {
    DCC_INTERACTION_ROUTE_F_UNSIGNED = 1U << 0,
    DCC_INTERACTION_ROUTE_F_PREFIX = 1U << 1
} dcc_interaction_route_flag_t;

typedef struct dcc_interaction_route {
    size_t size;
    const char *method;
    const char *path;
    uint32_t flags;
    dcc_interaction_server_cb callback;
    void *user_data;
} dcc_interaction_route_t;

DCC_API dcc_status_t dcc_interaction_server_add_route(
    dcc_interaction_server_t *server,
    const dcc_interaction_route_t *route
);
DCC_API dcc_status_t dcc_interaction_server_add_health_route(
    dcc_interaction_server_t *server,
    const char *path
);
DCC_API dcc_status_t dcc_interaction_server_clear_routes(dcc_interaction_server_t *server);

#ifdef __cplusplus
}
#endif

#endif
