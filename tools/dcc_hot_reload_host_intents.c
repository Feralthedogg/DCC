#include "internal/dcc_hot_reload_host.h"

typedef struct dcc_hot_reload_host_intent_name {
    const char *name;
    dcc_intents_t value;
} dcc_hot_reload_host_intent_name_t;

static const dcc_hot_reload_host_intent_name_t dcc_hot_reload_host_intents[] = {
    { "guilds", DCC_INTENT_GUILDS },
    { "guild_members", DCC_INTENT_GUILD_MEMBERS },
    { "guild_moderation", DCC_INTENT_GUILD_MODERATION },
    { "guild_expressions", DCC_INTENT_GUILD_EXPRESSIONS },
    { "guild_integrations", DCC_INTENT_GUILD_INTEGRATIONS },
    { "guild_webhooks", DCC_INTENT_GUILD_WEBHOOKS },
    { "guild_invites", DCC_INTENT_GUILD_INVITES },
    { "guild_voice_states", DCC_INTENT_GUILD_VOICE_STATES },
    { "guild_presences", DCC_INTENT_GUILD_PRESENCES },
    { "guild_messages", DCC_INTENT_GUILD_MESSAGES },
    { "guild_message_reactions", DCC_INTENT_GUILD_MESSAGE_REACTIONS },
    { "guild_message_typing", DCC_INTENT_GUILD_MESSAGE_TYPING },
    { "direct_messages", DCC_INTENT_DIRECT_MESSAGES },
    { "direct_message_reactions", DCC_INTENT_DIRECT_MESSAGE_REACTIONS },
    { "direct_message_typing", DCC_INTENT_DIRECT_MESSAGE_TYPING },
    { "message_content", DCC_INTENT_MESSAGE_CONTENT },
    { "guild_scheduled_events", DCC_INTENT_GUILD_SCHEDULED_EVENTS },
    { "auto_moderation_configuration", DCC_INTENT_AUTO_MODERATION_CONFIGURATION },
    { "auto_moderation_execution", DCC_INTENT_AUTO_MODERATION_EXECUTION },
    { "guild_message_polls", DCC_INTENT_GUILD_MESSAGE_POLLS },
    { "direct_message_polls", DCC_INTENT_DIRECT_MESSAGE_POLLS },
    { "default", DCC_INTENT_GUILDS },
};

dcc_intents_t dcc_hot_reload_host_all_intents(void) {
    return DCC_INTENTS_ALL;
}

int dcc_hot_reload_host_lookup_intent(const char *token, dcc_intents_t *out) {
    size_t i;
    if (out == NULL) {
        return -1;
    }
    for (i = 0; i < sizeof(dcc_hot_reload_host_intents) / sizeof(dcc_hot_reload_host_intents[0]); ++i) {
        if (dcc_hot_reload_host_token_matches(token, dcc_hot_reload_host_intents[i].name)) {
            *out = dcc_hot_reload_host_intents[i].value;
            return 0;
        }
    }
    return -1;
}
