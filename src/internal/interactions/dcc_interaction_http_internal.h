#ifndef DCC_INTERACTION_HTTP_INTERNAL_H
#define DCC_INTERACTION_HTTP_INTERNAL_H

#include "internal/interactions/dcc_interaction_request_state_internal.h"
#include "internal/interactions/dcc_interaction_server_state_internal.h"

#include <dcc/interaction_server.h>

#include <llam/io.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_interaction_read_request(
    dcc_interaction_server_t *server,
    llam_fd_t fd,
    dcc_interaction_request_t *request
);
int dcc_interaction_write_response(llam_fd_t fd, const dcc_interaction_request_t *request);

#ifdef __cplusplus
}
#endif

#endif
