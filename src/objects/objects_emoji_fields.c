#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>
#include <string.h>

void dcc_free_emoji_fields(dcc_emoji_t *emoji) {
    if (emoji == NULL) {
        return;
    }
    dcc_free_user_fields(&emoji->user);
    free((void *)emoji->role_ids);
    free((void *)emoji->name);
    emoji->role_ids = NULL;
    emoji->name = NULL;
}

int dcc_clone_emoji_fields(dcc_emoji_t *copy, const dcc_emoji_t *emoji) {
    if (copy == NULL || emoji == NULL) {
        return -1;
    }

    *copy = *emoji;
    copy->user.username = NULL;
    copy->user.global_name = NULL;
    copy->role_ids = NULL;
    copy->name = NULL;
    if (dcc_clone_user_fields(&copy->user, &emoji->user) != 0) {
        memset(copy, 0, sizeof(*copy));
        return -1;
    }
    copy->role_ids = dcc_clone_snowflake_array_or_null(emoji->role_ids, emoji->role_ids_count);
    copy->name = dcc_clone_string_or_null(emoji->name);
    if ((emoji->role_ids != NULL && emoji->role_ids_count > 0 && copy->role_ids == NULL) ||
        (emoji->name != NULL && copy->name == NULL)) {
        dcc_free_emoji_fields(copy);
        memset(copy, 0, sizeof(*copy));
        return -1;
    }

    return 0;
}
