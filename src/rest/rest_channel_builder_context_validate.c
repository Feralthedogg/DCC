#include "internal/rest/dcc_rest_channel_body_internal.h"
#include "internal/rest/dcc_rest_channel_builder_context_internal.h"
#include "internal/rest/dcc_rest_channel_forum_builders_internal.h"
#include "internal/rest/dcc_rest_config_internal.h"

#include <string.h>

#define DCC_CHANNEL_PARAMS_BASE_SIZE DCC_REST_FIELD_END(dcc_channel_params_t, lock_permissions)

static int dcc_rest_channel_type_valid(dcc_channel_type_t type) {
    switch (type) {
        case DCC_CHANNEL_TEXT:
        case DCC_CHANNEL_DM:
        case DCC_CHANNEL_VOICE:
        case DCC_CHANNEL_GROUP_DM:
        case DCC_CHANNEL_CATEGORY:
        case DCC_CHANNEL_ANNOUNCEMENT:
        case DCC_CHANNEL_STORE:
        case DCC_CHANNEL_ANNOUNCEMENT_THREAD:
        case DCC_CHANNEL_PUBLIC_THREAD:
        case DCC_CHANNEL_PRIVATE_THREAD:
        case DCC_CHANNEL_STAGE:
        case DCC_CHANNEL_DIRECTORY:
        case DCC_CHANNEL_FORUM:
        case DCC_CHANNEL_MEDIA:
            return 1;
        default:
            return 0;
    }
}

int dcc_rest_channel_archive_duration_valid(dcc_channel_auto_archive_duration_t duration) {
    return duration == DCC_CHANNEL_AUTO_ARCHIVE_1_HOUR ||
           duration == DCC_CHANNEL_AUTO_ARCHIVE_1_DAY ||
           duration == DCC_CHANNEL_AUTO_ARCHIVE_3_DAYS ||
           duration == DCC_CHANNEL_AUTO_ARCHIVE_1_WEEK;
}

int dcc_rest_channel_base_params_valid(const dcc_channel_params_t *params, uint8_t include_id) {
    return params != NULL &&
           params->size >= DCC_CHANNEL_PARAMS_BASE_SIZE &&
           params->guild_id != 0 &&
           (include_id == 0 || params->channel_id != 0) &&
           dcc_rest_channel_type_valid(params->type) &&
           params->name != NULL &&
           params->name[0] != '\0' &&
           strlen(params->name) <= 100U &&
           (params->permission_overwrite_count == 0 || params->permission_overwrites != NULL) &&
           params->default_sort_order <= 1U;
}

int dcc_rest_channel_forum_params_valid(const dcc_rest_channel_build_context_t *ctx) {
    if ((ctx->available_tag_count != 0 && ctx->available_tags == NULL) ||
        (ctx->default_reaction_emoji_id != 0 &&
         ctx->default_reaction_emoji_name != NULL &&
         ctx->default_reaction_emoji_name[0] != '\0') ||
        !dcc_rest_channel_forum_layout_valid(ctx->default_forum_layout)) {
        return 0;
    }

    for (size_t i = 0; i < ctx->available_tag_count; ++i) {
        if (!dcc_rest_channel_forum_tag_valid(&ctx->available_tags[i])) {
            return 0;
        }
    }
    return 1;
}
