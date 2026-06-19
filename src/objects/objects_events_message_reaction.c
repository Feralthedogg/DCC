#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>
#include <string.h>

dcc_message_reaction_t *dcc_message_reaction_clone(const dcc_message_reaction_t *reaction) {
    if (reaction == NULL) {
        return NULL;
    }

    dcc_message_reaction_t *copy = (dcc_message_reaction_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }

    *copy = *reaction;
    memset(&copy->user, 0, sizeof(copy->user));
    memset(&copy->member, 0, sizeof(copy->member));
    memset(&copy->emoji, 0, sizeof(copy->emoji));
    copy->emoji_name = NULL;
    if (dcc_clone_user_fields(&copy->user, &reaction->user) != 0) {
        dcc_message_reaction_free(copy);
        return NULL;
    }
    if (dcc_clone_member_fields(&copy->member, &reaction->member) != 0) {
        dcc_message_reaction_free(copy);
        return NULL;
    }
    if (dcc_clone_emoji_fields(&copy->emoji, &reaction->emoji) != 0) {
        dcc_message_reaction_free(copy);
        return NULL;
    }
    copy->emoji_name = dcc_clone_string_or_null(reaction->emoji_name);
    if (reaction->emoji_name != NULL && copy->emoji_name == NULL) {
        dcc_message_reaction_free(copy);
        return NULL;
    }

    return copy;
}

void dcc_message_reaction_free(dcc_message_reaction_t *reaction) {
    if (reaction == NULL) {
        return;
    }
    dcc_free_user_fields(&reaction->user);
    dcc_free_member_fields(&reaction->member);
    dcc_free_emoji_fields(&reaction->emoji);
    free((void *)reaction->emoji_name);
    free(reaction);
}
