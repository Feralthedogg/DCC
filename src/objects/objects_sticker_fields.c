#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>
#include <string.h>

void dcc_free_sticker_fields(dcc_sticker_t *sticker) {
    if (sticker == NULL) {
        return;
    }
    dcc_free_user_fields(&sticker->user);
    free((void *)sticker->name);
    free((void *)sticker->description);
    free((void *)sticker->tags);
    free((void *)sticker->asset);
    sticker->name = NULL;
    sticker->description = NULL;
    sticker->tags = NULL;
    sticker->asset = NULL;
}

int dcc_clone_sticker_fields(dcc_sticker_t *copy, const dcc_sticker_t *sticker) {
    if (copy == NULL || sticker == NULL) {
        return -1;
    }

    *copy = *sticker;
    copy->user.username = NULL;
    copy->user.global_name = NULL;
    copy->name = NULL;
    copy->description = NULL;
    copy->tags = NULL;
    copy->asset = NULL;
    if (dcc_clone_user_fields(&copy->user, &sticker->user) != 0) {
        memset(copy, 0, sizeof(*copy));
        return -1;
    }
    copy->name = dcc_clone_string_or_null(sticker->name);
    copy->description = dcc_clone_string_or_null(sticker->description);
    copy->tags = dcc_clone_string_or_null(sticker->tags);
    copy->asset = dcc_clone_string_or_null(sticker->asset);
    if ((sticker->name != NULL && copy->name == NULL) ||
        (sticker->description != NULL && copy->description == NULL) ||
        (sticker->tags != NULL && copy->tags == NULL) ||
        (sticker->asset != NULL && copy->asset == NULL)) {
        dcc_free_sticker_fields(copy);
        memset(copy, 0, sizeof(*copy));
        return -1;
    }

    return 0;
}
