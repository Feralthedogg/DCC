#ifndef DCC_INTERACTION_SERVER_CORE_INTERNAL_H
#define DCC_INTERACTION_SERVER_CORE_INTERNAL_H

#include "internal/interactions/dcc_interaction_server_state_internal.h"

#include <dcc/interaction_server.h>

#ifdef __cplusplus
extern "C" {
#endif

void dcc_interaction_server_set_error(dcc_interaction_server_t *server, const char *message);
void dcc_interaction_server_close_listener(dcc_interaction_server_t *server);
void dcc_interaction_server_fill_state(
    const dcc_interaction_server_t *server,
    dcc_interaction_server_state_t *out
);

#ifdef __cplusplus
}
#endif

#endif
