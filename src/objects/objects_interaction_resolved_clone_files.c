#include "internal/objects/dcc_interaction_resolved_clone_internal.h"

#include <stdlib.h>
#include <string.h>

int dcc_clone_interaction_resolved_files(
    dcc_interaction_resolved_t *copy,
    const dcc_interaction_resolved_t *resolved
) {
    if (resolved->attachments_count > 0) {
        if (resolved->attachments == NULL || resolved->attachments_count > (size_t)-1 / sizeof(dcc_attachment_t)) {
            return -1;
        }
        dcc_attachment_t *attachments = (dcc_attachment_t *)calloc(
            resolved->attachments_count,
            sizeof(*attachments)
        );
        if (attachments == NULL) {
            return -1;
        }
        copy->attachments = attachments;
        for (size_t i = 0; i < resolved->attachments_count; i++) {
            if (dcc_clone_attachment_fields(&attachments[i], &resolved->attachments[i]) != 0) {
                return -1;
            }
        }
    }

    if (resolved->member_permissions_count > 0) {
        if (resolved->member_permissions == NULL ||
            resolved->member_permissions_count >
                (size_t)-1 / sizeof(dcc_interaction_resolved_permission_t)) {
            return -1;
        }
        dcc_interaction_resolved_permission_t *permissions =
            (dcc_interaction_resolved_permission_t *)calloc(
                resolved->member_permissions_count,
                sizeof(*permissions)
            );
        if (permissions == NULL) {
            return -1;
        }
        memcpy(
            permissions,
            resolved->member_permissions,
            resolved->member_permissions_count * sizeof(*permissions)
        );
        copy->member_permissions = permissions;
    }

    return 0;
}
