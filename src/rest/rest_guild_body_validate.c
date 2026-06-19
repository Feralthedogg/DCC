#include "internal/rest/dcc_rest_guild_body_internal.h"

static int dcc_rest_guild_afk_timeout_valid(dcc_guild_afk_timeout_t value) {
    return value == DCC_GUILD_AFK_OFF ||
           value == DCC_GUILD_AFK_60 ||
           value == DCC_GUILD_AFK_300 ||
           value == DCC_GUILD_AFK_900 ||
           value == DCC_GUILD_AFK_1800 ||
           value == DCC_GUILD_AFK_3600;
}

int dcc_rest_guild_params_valid(const dcc_guild_params_t *params, int with_id) {
    return params != NULL &&
           params->size >= sizeof(*params) &&
           (!with_id || params->guild_id != 0) &&
           params->default_message_notifications <= DCC_GUILD_NOTIFY_ONLY_MENTIONS &&
           params->explicit_content_filter <= DCC_GUILD_EXPLICIT_CONTENT_ALL_MEMBERS &&
           params->mfa_level <= DCC_GUILD_MFA_ELEVATED &&
           dcc_rest_guild_afk_timeout_valid(params->afk_timeout);
}
