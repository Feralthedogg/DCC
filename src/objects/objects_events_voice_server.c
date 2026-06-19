#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>

dcc_voice_server_update_t *dcc_voice_server_update_clone(const dcc_voice_server_update_t *voice_server) {
    if (voice_server == NULL) {
        return NULL;
    }

    dcc_voice_server_update_t *copy = (dcc_voice_server_update_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }

    *copy = *voice_server;
    copy->token = dcc_clone_string_or_null(voice_server->token);
    copy->endpoint = dcc_clone_string_or_null(voice_server->endpoint);
    if ((voice_server->token != NULL && copy->token == NULL) ||
        (voice_server->endpoint != NULL && copy->endpoint == NULL)) {
        dcc_voice_server_update_free(copy);
        return NULL;
    }

    return copy;
}

void dcc_voice_server_update_free(dcc_voice_server_update_t *voice_server) {
    if (voice_server == NULL) {
        return;
    }
    free((void *)voice_server->token);
    free((void *)voice_server->endpoint);
    free(voice_server);
}
