#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>
#include <string.h>

void dcc_free_attachment_fields(dcc_attachment_t *attachment) {
    if (attachment == NULL) {
        return;
    }
    free((void *)attachment->filename);
    free((void *)attachment->description);
    free((void *)attachment->url);
    free((void *)attachment->proxy_url);
    free((void *)attachment->content_type);
    free((void *)attachment->waveform);
    attachment->filename = NULL;
    attachment->description = NULL;
    attachment->url = NULL;
    attachment->proxy_url = NULL;
    attachment->content_type = NULL;
    attachment->waveform = NULL;
}

int dcc_clone_attachment_fields(dcc_attachment_t *copy, const dcc_attachment_t *attachment) {
    if (copy == NULL || attachment == NULL) {
        return -1;
    }

    *copy = *attachment;
    copy->filename = dcc_clone_string_or_null(attachment->filename);
    copy->description = dcc_clone_string_or_null(attachment->description);
    copy->url = dcc_clone_string_or_null(attachment->url);
    copy->proxy_url = dcc_clone_string_or_null(attachment->proxy_url);
    copy->content_type = dcc_clone_string_or_null(attachment->content_type);
    copy->waveform = dcc_clone_string_or_null(attachment->waveform);
    if ((attachment->filename != NULL && copy->filename == NULL) ||
        (attachment->description != NULL && copy->description == NULL) ||
        (attachment->url != NULL && copy->url == NULL) ||
        (attachment->proxy_url != NULL && copy->proxy_url == NULL) ||
        (attachment->content_type != NULL && copy->content_type == NULL) ||
        (attachment->waveform != NULL && copy->waveform == NULL)) {
        dcc_free_attachment_fields(copy);
        memset(copy, 0, sizeof(*copy));
        return -1;
    }

    return 0;
}
