#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>

dcc_guild_join_request_update_t *dcc_guild_join_request_update_clone(
    const dcc_guild_join_request_update_t *update
) {
    if (update == NULL) {
        return NULL;
    }

    dcc_guild_join_request_update_t *copy = (dcc_guild_join_request_update_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }
    *copy = *update;
    copy->status = dcc_clone_string_or_null(update->status);
    if (update->status != NULL && copy->status == NULL) {
        dcc_guild_join_request_update_free(copy);
        return NULL;
    }
    return copy;
}

void dcc_guild_join_request_update_free(dcc_guild_join_request_update_t *update) {
    if (update == NULL) {
        return;
    }
    free((void *)update->status);
    free(update);
}
