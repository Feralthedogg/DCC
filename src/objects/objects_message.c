#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>

dcc_attachment_t *dcc_attachment_clone(const dcc_attachment_t *attachment) {
    if (attachment == NULL) {
        return NULL;
    }

    dcc_attachment_t *copy = (dcc_attachment_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }
    if (dcc_clone_attachment_fields(copy, attachment) != 0) {
        free(copy);
        return NULL;
    }

    return copy;
}

void dcc_attachment_free(dcc_attachment_t *attachment) {
    if (attachment == NULL) {
        return;
    }
    dcc_free_attachment_fields(attachment);
    free(attachment);
}

dcc_message_t *dcc_message_clone(const dcc_message_t *message) {
    if (message == NULL) {
        return NULL;
    }

    dcc_message_t *copy = (dcc_message_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }

    *copy = *message;
    copy->author.username = dcc_clone_string_or_null(message->author.username);
    copy->author.global_name = dcc_clone_string_or_null(message->author.global_name);
    copy->author.avatar = dcc_clone_string_or_null(message->author.avatar);
    copy->content = dcc_clone_string_or_null(message->content);
    copy->timestamp = dcc_clone_string_or_null(message->timestamp);
    copy->edited_timestamp = dcc_clone_string_or_null(message->edited_timestamp);

    if ((message->author.username != NULL && copy->author.username == NULL) ||
        (message->author.global_name != NULL && copy->author.global_name == NULL) ||
        (message->author.avatar != NULL && copy->author.avatar == NULL) ||
        (message->content != NULL && copy->content == NULL) ||
        (message->timestamp != NULL && copy->timestamp == NULL) ||
        (message->edited_timestamp != NULL && copy->edited_timestamp == NULL)) {
        dcc_message_free(copy);
        return NULL;
    }

    return copy;
}

void dcc_message_free(dcc_message_t *message) {
    if (message == NULL) {
        return;
    }
    free((void *)message->author.username);
    free((void *)message->author.global_name);
    free((void *)message->author.avatar);
    free((void *)message->content);
    free((void *)message->timestamp);
    free((void *)message->edited_timestamp);
    free(message);
}
