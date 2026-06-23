#ifndef DCC_SUGAR_APP_OPTIONS_H
#define DCC_SUGAR_APP_OPTIONS_H

#include <dcc/app.h>
#include <dcc/intents.h>
#include <dcc/sugar/app_definition.h>

#define DCC_SUGAR_APP_DEFAULT_INTENTS DCC_INTENT_GUILDS

#define DCC_SUGAR_APP_CLIENT_OPTIONS_WITH_GUILD_INFERENCE(token_, intents_) \
    ((dcc_client_options_t){ \
        .size = sizeof(dcc_client_options_t), \
        .token = (token_), \
        .intents = (intents_), \
        .enable_cache = 1U, \
        .infer_guild_id_from_channel = 1U \
    })

#define DCC_APP_OPTIONS_PRESET_BOT(token_) \
    ((dcc_app_options_t){ \
        .size = sizeof(dcc_app_options_t), \
        .client = DCC_SUGAR_APP_CLIENT_OPTIONS_WITH_GUILD_INFERENCE((token_), DCC_SUGAR_APP_DEFAULT_INTENTS), \
        .command_registry = DCC_COMMAND_REGISTRY_OPTIONS_GLOBAL() \
    })

#define DCC_APP_OPTIONS_PRESET_BOT_INTENTS(token_, intents_) \
    ((dcc_app_options_t){ \
        .size = sizeof(dcc_app_options_t), \
        .client = DCC_SUGAR_APP_CLIENT_OPTIONS_WITH_GUILD_INFERENCE((token_), (intents_)), \
        .command_registry = DCC_COMMAND_REGISTRY_OPTIONS_GLOBAL() \
    })

#define DCC_APP_OPTIONS_BOT(token_) \
    DCC_APP_OPTIONS_PRESET_BOT((token_))

#define DCC_APP_OPTIONS_BOT_INTENTS(token_, intents_) \
    DCC_APP_OPTIONS_PRESET_BOT_INTENTS((token_), (intents_))

#define DCC_APP_OPTIONS_PRESET_DEV(token_) \
    ((dcc_app_options_t){ \
        .size = sizeof(dcc_app_options_t), \
        .client = DCC_SUGAR_APP_CLIENT_OPTIONS_WITH_GUILD_INFERENCE((token_), DCC_SUGAR_APP_DEFAULT_INTENTS), \
        .command_registry = DCC_COMMAND_REGISTRY_OPTIONS_GLOBAL(), \
        .auto_defer_after_ms = 1500U, \
        .auto_defer_ephemeral = 1U, \
        .command_sync = DCC_APP_COMMAND_SYNC_AUTO(), \
        .command_sync_on_ready = 1U \
    })

#define DCC_APP_OPTIONS_PRESET_DEV_INTENTS(token_, intents_) \
    ((dcc_app_options_t){ \
        .size = sizeof(dcc_app_options_t), \
        .client = DCC_SUGAR_APP_CLIENT_OPTIONS_WITH_GUILD_INFERENCE((token_), (intents_)), \
        .command_registry = DCC_COMMAND_REGISTRY_OPTIONS_GLOBAL(), \
        .auto_defer_after_ms = 1500U, \
        .auto_defer_ephemeral = 1U, \
        .command_sync = DCC_APP_COMMAND_SYNC_AUTO(), \
        .command_sync_on_ready = 1U \
    })

#define DCC_APP_OPTIONS_DEV(token_) \
    DCC_APP_OPTIONS_PRESET_DEV((token_))

#define DCC_APP_OPTIONS_DEV_INTENTS(token_, intents_) \
    DCC_APP_OPTIONS_PRESET_DEV_INTENTS((token_), (intents_))

#define DCC_APP_OPTIONS_PRESET_DEV_GUILD(token_, guild_id_) \
    ((dcc_app_options_t){ \
        .size = sizeof(dcc_app_options_t), \
        .client = DCC_SUGAR_APP_CLIENT_OPTIONS_WITH_GUILD_INFERENCE((token_), DCC_SUGAR_APP_DEFAULT_INTENTS), \
        .command_registry = DCC_COMMAND_REGISTRY_OPTIONS_GUILD((guild_id_)), \
        .auto_defer_after_ms = 1500U, \
        .auto_defer_ephemeral = 1U, \
        .command_sync = DCC_APP_COMMAND_SYNC_GUILD_AUTO((guild_id_)), \
        .command_sync_on_ready = 1U \
    })

#define DCC_APP_OPTIONS_PRESET_DEV_GUILD_INTENTS(token_, intents_, guild_id_) \
    ((dcc_app_options_t){ \
        .size = sizeof(dcc_app_options_t), \
        .client = DCC_SUGAR_APP_CLIENT_OPTIONS_WITH_GUILD_INFERENCE((token_), (intents_)), \
        .command_registry = DCC_COMMAND_REGISTRY_OPTIONS_GUILD((guild_id_)), \
        .auto_defer_after_ms = 1500U, \
        .auto_defer_ephemeral = 1U, \
        .command_sync = DCC_APP_COMMAND_SYNC_GUILD_AUTO((guild_id_)), \
        .command_sync_on_ready = 1U \
    })

#define DCC_APP_OPTIONS_DEV_GUILD(token_, guild_id_) \
    DCC_APP_OPTIONS_PRESET_DEV_GUILD((token_), (guild_id_))

#define DCC_APP_OPTIONS_DEV_GUILD_INTENTS(token_, intents_, guild_id_) \
    DCC_APP_OPTIONS_PRESET_DEV_GUILD_INTENTS((token_), (intents_), (guild_id_))

#endif
