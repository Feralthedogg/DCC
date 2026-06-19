#ifndef DCC_INTERACTION_RUNTIME_INTERNAL_H
#define DCC_INTERACTION_RUNTIME_INTERNAL_H

#include "internal/interactions/dcc_interaction_server_state_internal.h"

#include <dcc/interaction_server.h>

#include <llam/io.h>

typedef struct dcc_interaction_client_task {
    dcc_interaction_server_t *server;
    llam_fd_t fd;
} dcc_interaction_client_task_t;

#ifdef __cplusplus
extern "C" {
#endif

void dcc_interaction_client_task_run(void *arg);
void dcc_interaction_accept_task(void *arg);
dcc_status_t dcc_interaction_bind_listener(dcc_interaction_server_t *server, llam_fd_t *out_fd);

#ifdef __cplusplus
}
#endif

#endif
