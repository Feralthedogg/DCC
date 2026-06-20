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

    if (dcc_clone_message_fields(copy, message) != 0) {
        free(copy);
        return NULL;
    }

    return copy;
}

void dcc_message_free(dcc_message_t *message) {
    if (message == NULL) {
        return;
    }
    dcc_free_message_fields(message);
    free(message);
}
