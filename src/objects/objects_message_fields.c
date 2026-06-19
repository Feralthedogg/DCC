#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>
#include <string.h>

void dcc_free_message_fields(dcc_message_t *message) {
    if (message == NULL) {
        return;
    }
    dcc_free_user_fields(&message->author);
    free((void *)message->content);
    free((void *)message->timestamp);
    free((void *)message->edited_timestamp);
    message->content = NULL;
    message->timestamp = NULL;
    message->edited_timestamp = NULL;
}

int dcc_clone_message_fields(dcc_message_t *copy, const dcc_message_t *message) {
    if (copy == NULL || message == NULL) {
        return -1;
    }

    *copy = *message;
    memset(&copy->author, 0, sizeof(copy->author));
    copy->content = NULL;
    copy->timestamp = NULL;
    copy->edited_timestamp = NULL;
    if (dcc_clone_user_fields(&copy->author, &message->author) != 0) {
        memset(copy, 0, sizeof(*copy));
        return -1;
    }
    copy->content = dcc_clone_string_or_null(message->content);
    copy->timestamp = dcc_clone_string_or_null(message->timestamp);
    copy->edited_timestamp = dcc_clone_string_or_null(message->edited_timestamp);
    if ((message->content != NULL && copy->content == NULL) ||
        (message->timestamp != NULL && copy->timestamp == NULL) ||
        (message->edited_timestamp != NULL && copy->edited_timestamp == NULL)) {
        dcc_free_message_fields(copy);
        memset(copy, 0, sizeof(*copy));
        return -1;
    }

    return 0;
}
