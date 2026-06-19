#include "internal/cache/dcc_cache_merge_guild_internal.h"
#include "internal/dcc_cache_fields.h"

void dcc_cache_merge_guild_flag_fields(dcc_guild_t *merged, const dcc_guild_t *existing, uint64_t fields) {
    if ((fields & DCC_CACHE_FIELD_GUILD_FLAGS) == 0 && merged->flags == 0) {
        merged->flags = existing->flags;
    }
    if ((fields & DCC_CACHE_FIELD_GUILD_UNAVAILABLE) == 0 && merged->unavailable == 0) {
        merged->unavailable = existing->unavailable;
    }
    if ((fields & DCC_CACHE_FIELD_GUILD_LARGE) == 0 && merged->large == 0) {
        merged->large = existing->large;
    }
    if ((fields & DCC_CACHE_FIELD_GUILD_WIDGET_ENABLED) == 0 && merged->widget_enabled == 0) {
        merged->widget_enabled = existing->widget_enabled;
    }
    if ((fields & DCC_CACHE_FIELD_GUILD_PREMIUM_PROGRESS_BAR_ENABLED) == 0 &&
        merged->premium_progress_bar_enabled == 0) {
        merged->premium_progress_bar_enabled = existing->premium_progress_bar_enabled;
    }
}
