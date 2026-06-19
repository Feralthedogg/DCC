#include "internal/objects/dcc_interaction_resolved_clone_internal.h"

int dcc_clone_interaction_resolved_fields(
    dcc_interaction_resolved_t *copy,
    const dcc_interaction_resolved_t *resolved
) {
    if (copy == NULL || resolved == NULL) {
        return -1;
    }

    *copy = *resolved;
    copy->users = NULL;
    copy->members = NULL;
    copy->roles = NULL;
    copy->channels = NULL;
    copy->messages = NULL;
    copy->attachments = NULL;
    copy->member_permissions = NULL;

    if (dcc_clone_interaction_resolved_people(copy, resolved) != 0 ||
        dcc_clone_interaction_resolved_resources(copy, resolved) != 0 ||
        dcc_clone_interaction_resolved_files(copy, resolved) != 0) {
        dcc_free_interaction_resolved_fields(copy);
        return -1;
    }

    return 0;
}
