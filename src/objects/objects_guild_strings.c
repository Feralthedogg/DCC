#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>

int dcc_clone_guild_string_fields(dcc_guild_t *copy, const dcc_guild_t *guild) {
    copy->name = dcc_clone_string_or_null(guild->name);
    copy->description = dcc_clone_string_or_null(guild->description);
    copy->icon = dcc_clone_string_or_null(guild->icon);
    copy->banner = dcc_clone_string_or_null(guild->banner);
    copy->vanity_url_code = dcc_clone_string_or_null(guild->vanity_url_code);
    copy->preferred_locale = dcc_clone_string_or_null(guild->preferred_locale);
    copy->splash = dcc_clone_string_or_null(guild->splash);
    copy->discovery_splash = dcc_clone_string_or_null(guild->discovery_splash);

    if ((guild->name != NULL && copy->name == NULL) ||
        (guild->description != NULL && copy->description == NULL) ||
        (guild->icon != NULL && copy->icon == NULL) ||
        (guild->banner != NULL && copy->banner == NULL) ||
        (guild->vanity_url_code != NULL && copy->vanity_url_code == NULL) ||
        (guild->preferred_locale != NULL && copy->preferred_locale == NULL) ||
        (guild->splash != NULL && copy->splash == NULL) ||
        (guild->discovery_splash != NULL && copy->discovery_splash == NULL)) {
        return -1;
    }

    return 0;
}

void dcc_free_guild_string_fields(dcc_guild_t *guild) {
    free((void *)guild->name);
    free((void *)guild->description);
    free((void *)guild->icon);
    free((void *)guild->banner);
    free((void *)guild->vanity_url_code);
    free((void *)guild->preferred_locale);
    free((void *)guild->splash);
    free((void *)guild->discovery_splash);
}
