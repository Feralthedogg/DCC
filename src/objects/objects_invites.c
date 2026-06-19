#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>
#include <string.h>

dcc_invite_t *dcc_invite_clone(const dcc_invite_t *invite) {
    if (invite == NULL) {
        return NULL;
    }

    dcc_invite_t *copy = (dcc_invite_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }

    *copy = *invite;
    memset(&copy->inviter, 0, sizeof(copy->inviter));
    copy->code = dcc_clone_string_or_null(invite->code);
    copy->created_at = dcc_clone_string_or_null(invite->created_at);
    copy->expires_at = dcc_clone_string_or_null(invite->expires_at);

    if (dcc_clone_user_fields(&copy->inviter, &invite->inviter) != 0 ||
        (invite->code != NULL && copy->code == NULL) ||
        (invite->created_at != NULL && copy->created_at == NULL) ||
        (invite->expires_at != NULL && copy->expires_at == NULL)) {
        dcc_invite_free(copy);
        return NULL;
    }

    return copy;
}

void dcc_invite_free(dcc_invite_t *invite) {
    if (invite == NULL) {
        return;
    }
    dcc_free_user_fields(&invite->inviter);
    free((void *)invite->code);
    free((void *)invite->created_at);
    free((void *)invite->expires_at);
    free(invite);
}
