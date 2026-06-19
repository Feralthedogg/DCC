#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>

static void dcc_guild_clear_owned_refs(dcc_guild_t *guild) {
    guild->name = NULL;
    guild->description = NULL;
    guild->icon = NULL;
    guild->banner = NULL;
    guild->vanity_url_code = NULL;
    guild->preferred_locale = NULL;
    guild->splash = NULL;
    guild->discovery_splash = NULL;
    guild->roles = NULL;
    guild->channels = NULL;
    guild->threads = NULL;
    guild->scheduled_events = NULL;
    guild->stage_instances = NULL;
    guild->presences = NULL;
    guild->emojis = NULL;
    guild->stickers = NULL;
    guild->soundboard_sounds = NULL;
}

dcc_guild_t *dcc_guild_clone(const dcc_guild_t *guild) {
    if (guild == NULL) {
        return NULL;
    }

    dcc_guild_t *copy = (dcc_guild_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }

    *copy = *guild;
    dcc_guild_clear_owned_refs(copy);
    if (dcc_clone_guild_string_fields(copy, guild) != 0 ||
        dcc_clone_guild_collection_fields(copy, guild) != 0) {
        dcc_guild_free(copy);
        return NULL;
    }

    return copy;
}

void dcc_guild_free(dcc_guild_t *guild) {
    if (guild == NULL) {
        return;
    }
    dcc_free_guild_string_fields(guild);
    dcc_free_guild_collection_fields(guild);
    free(guild);
}
