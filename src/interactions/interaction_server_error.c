#include "internal/interactions/dcc_interaction_server_internal.h"

#include <stdio.h>

void dcc_interaction_server_set_error(dcc_interaction_server_t *server, const char *message) {
    if (server == NULL) {
        return;
    }
    if (message == NULL) {
        server->last_error[0] = '\0';
        return;
    }
    snprintf(server->last_error, sizeof(server->last_error), "%s", message);
}
