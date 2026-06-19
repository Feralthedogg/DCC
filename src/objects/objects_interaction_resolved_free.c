#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>
#include <string.h>

void dcc_free_interaction_resolved_fields(dcc_interaction_resolved_t *resolved) {
    if (resolved == NULL) {
        return;
    }

    if (resolved->users != NULL) {
        dcc_user_t *users = (dcc_user_t *)resolved->users;
        for (size_t i = 0; i < resolved->users_count; i++) {
            dcc_free_user_fields(&users[i]);
        }
        free(users);
    }
    if (resolved->members != NULL) {
        dcc_member_t *members = (dcc_member_t *)resolved->members;
        for (size_t i = 0; i < resolved->members_count; i++) {
            dcc_free_member_fields(&members[i]);
        }
        free(members);
    }
    if (resolved->roles != NULL) {
        dcc_role_t *roles = (dcc_role_t *)resolved->roles;
        for (size_t i = 0; i < resolved->roles_count; i++) {
            dcc_free_role_fields(&roles[i]);
        }
        free(roles);
    }
    if (resolved->channels != NULL) {
        dcc_channel_t *channels = (dcc_channel_t *)resolved->channels;
        for (size_t i = 0; i < resolved->channels_count; i++) {
            dcc_free_channel_fields(&channels[i]);
        }
        free(channels);
    }
    if (resolved->messages != NULL) {
        dcc_message_t *messages = (dcc_message_t *)resolved->messages;
        for (size_t i = 0; i < resolved->messages_count; i++) {
            dcc_free_message_fields(&messages[i]);
        }
        free(messages);
    }
    if (resolved->attachments != NULL) {
        dcc_attachment_t *attachments = (dcc_attachment_t *)resolved->attachments;
        for (size_t i = 0; i < resolved->attachments_count; i++) {
            dcc_free_attachment_fields(&attachments[i]);
        }
        free(attachments);
    }
    free((void *)resolved->member_permissions);
    memset(resolved, 0, sizeof(*resolved));
}
