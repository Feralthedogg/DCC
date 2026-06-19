#include "internal/cache/dcc_cache_merge_guild_internal.h"

void dcc_cache_merge_guild_string_fields(dcc_guild_t *merged, const dcc_guild_t *existing) {
    if (merged->name == NULL) {
        merged->name = existing->name;
    }
    if (merged->description == NULL) {
        merged->description = existing->description;
    }
    if (merged->icon == NULL) {
        merged->icon = existing->icon;
    }
    if (merged->banner == NULL) {
        merged->banner = existing->banner;
    }
    if (merged->vanity_url_code == NULL) {
        merged->vanity_url_code = existing->vanity_url_code;
    }
    if (merged->preferred_locale == NULL) {
        merged->preferred_locale = existing->preferred_locale;
    }
    if (merged->splash == NULL) {
        merged->splash = existing->splash;
    }
    if (merged->discovery_splash == NULL) {
        merged->discovery_splash = existing->discovery_splash;
    }
}
