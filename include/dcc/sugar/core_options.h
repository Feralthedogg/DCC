/* Included by <dcc/sugar.h>. */
#define DCC_ARRAY_LEN(array_) (sizeof(array_) / sizeof((array_)[0]))
#define DCC_SUGAR_ARRAY(type_, ...) ((type_[]){ __VA_ARGS__ })
#define DCC_SUGAR_ARRAY_LEN(type_, ...) (sizeof((type_[]){ __VA_ARGS__ }) / sizeof(type_))
#define DCC_SUGAR_PTR(type_, value_) (&DCC_SUGAR_ARRAY(type_, value_)[0])

#ifndef DCC_MESSAGE_FLAG_SUPPRESS_EMBEDS
#define DCC_MESSAGE_FLAG_SUPPRESS_EMBEDS UINT64_C(4)
#endif
#ifndef DCC_MESSAGE_FLAG_EPHEMERAL
#define DCC_MESSAGE_FLAG_EPHEMERAL UINT64_C(64)
#endif
#ifndef DCC_MESSAGE_FLAG_LOADING
#define DCC_MESSAGE_FLAG_LOADING UINT64_C(128)
#endif
#ifndef DCC_MESSAGE_FLAG_SUPPRESS_NOTIFICATIONS
#define DCC_MESSAGE_FLAG_SUPPRESS_NOTIFICATIONS UINT64_C(4096)
#endif

#define DCC_INTENTS_NONE ((dcc_intents_t)0)
#define DCC_INTENTS_DEFAULT (DCC_INTENT_GUILDS)
#define DCC_INTENTS_GUILD_MESSAGES (DCC_INTENT_GUILDS | DCC_INTENT_GUILD_MESSAGES)
#define DCC_INTENTS_DIRECT_MESSAGES (DCC_INTENT_DIRECT_MESSAGES)
#define DCC_INTENTS_MESSAGES \
    (DCC_INTENT_GUILDS | DCC_INTENT_GUILD_MESSAGES | DCC_INTENT_DIRECT_MESSAGES)
#define DCC_INTENTS_MESSAGE_REACTIONS \
    (DCC_INTENT_GUILD_MESSAGE_REACTIONS | DCC_INTENT_DIRECT_MESSAGE_REACTIONS)
#define DCC_INTENTS_MESSAGE_TYPING \
    (DCC_INTENT_GUILD_MESSAGE_TYPING | DCC_INTENT_DIRECT_MESSAGE_TYPING)
#define DCC_INTENTS_MESSAGE_POLLS \
    (DCC_INTENT_GUILD_MESSAGE_POLLS | DCC_INTENT_DIRECT_MESSAGE_POLLS)
#define DCC_INTENTS_PRIVILEGED \
    (DCC_INTENT_GUILD_MEMBERS | DCC_INTENT_GUILD_PRESENCES | DCC_INTENT_MESSAGE_CONTENT)
#define DCC_INTENTS_UNPRIVILEGED \
    (DCC_INTENT_GUILDS | \
     DCC_INTENT_GUILD_MODERATION | \
     DCC_INTENT_GUILD_EXPRESSIONS | \
     DCC_INTENT_GUILD_INTEGRATIONS | \
     DCC_INTENT_GUILD_WEBHOOKS | \
     DCC_INTENT_GUILD_INVITES | \
     DCC_INTENT_GUILD_VOICE_STATES | \
     DCC_INTENT_GUILD_MESSAGES | \
     DCC_INTENT_GUILD_MESSAGE_REACTIONS | \
     DCC_INTENT_GUILD_MESSAGE_TYPING | \
     DCC_INTENT_DIRECT_MESSAGES | \
     DCC_INTENT_DIRECT_MESSAGE_REACTIONS | \
     DCC_INTENT_DIRECT_MESSAGE_TYPING | \
     DCC_INTENT_GUILD_SCHEDULED_EVENTS | \
     DCC_INTENT_AUTO_MODERATION_CONFIGURATION | \
     DCC_INTENT_AUTO_MODERATION_EXECUTION | \
     DCC_INTENT_GUILD_MESSAGE_POLLS | \
     DCC_INTENT_DIRECT_MESSAGE_POLLS)

#define DCC_CLIENT_OPTIONS(token_, intents_) \
    ((dcc_client_options_t){ \
        .size = sizeof(dcc_client_options_t), \
        .token = (token_), \
        .intents = (intents_) \
    })

#define DCC_CLIENT_OPTIONS_ALL(token_) \
    DCC_CLIENT_OPTIONS((token_), DCC_INTENTS_ALL)

#define DCC_CLIENT_SHARDED_OPTIONS(token_, intents_, shard_id_, shard_count_) \
    ((dcc_client_options_t){ \
        .size = sizeof(dcc_client_options_t), \
        .token = (token_), \
        .intents = (intents_), \
        .shard_id = (shard_id_), \
        .shard_count = (shard_count_) \
    })

#define DCC_CLIENT_SHARDED_OPTIONS_ALL(token_, shard_id_, shard_count_) \
    DCC_CLIENT_SHARDED_OPTIONS((token_), DCC_INTENTS_ALL, (shard_id_), (shard_count_))

#define DCC_CLIENT_OPTIONS_WITH_GUILD_INFERENCE(token_, intents_) \
    ((dcc_client_options_t){ \
        .size = sizeof(dcc_client_options_t), \
        .token = (token_), \
        .intents = (intents_), \
        .enable_cache = 1U, \
        .infer_guild_id_from_channel = 1U \
    })

#define DCC_CLIENT_OPTIONS_WITH_GUILD_INFERENCE_ALL(token_) \
    DCC_CLIENT_OPTIONS_WITH_GUILD_INFERENCE((token_), DCC_INTENTS_ALL)

#define DCC_APP_OPTIONS(token_, intents_) \
    ((dcc_app_options_t){ \
        .size = sizeof(dcc_app_options_t), \
        .client = DCC_CLIENT_OPTIONS((token_), (intents_)), \
        .command_registry = DCC_COMMAND_REGISTRY_OPTIONS_GLOBAL() \
    })

#define DCC_APP_OPTIONS_ALL(token_) \
    DCC_APP_OPTIONS((token_), DCC_INTENTS_ALL)

#define DCC_APP_OPTIONS_GUILD(token_, intents_, guild_id_) \
    ((dcc_app_options_t){ \
        .size = sizeof(dcc_app_options_t), \
        .client = DCC_CLIENT_OPTIONS((token_), (intents_)), \
        .command_registry = DCC_COMMAND_REGISTRY_OPTIONS_GUILD(guild_id_) \
    })

#define DCC_APP_OPTIONS_GUILD_ALL(token_, guild_id_) \
    DCC_APP_OPTIONS_GUILD((token_), DCC_INTENTS_ALL, (guild_id_))

#define DCC_APP_OPTIONS_WITH_GUILD_INFERENCE(token_, intents_) \
    ((dcc_app_options_t){ \
        .size = sizeof(dcc_app_options_t), \
        .client = DCC_CLIENT_OPTIONS_WITH_GUILD_INFERENCE((token_), (intents_)), \
        .command_registry = DCC_COMMAND_REGISTRY_OPTIONS_GLOBAL() \
    })

#define DCC_APP_OPTIONS_WITH_GUILD_INFERENCE_ALL(token_) \
    DCC_APP_OPTIONS_WITH_GUILD_INFERENCE((token_), DCC_INTENTS_ALL)

#include <dcc/sugar/app_options.h>

#define DCC_APP_OPTIONS_WITH_STORE(token_, intents_, store_file_path_) \
    ((dcc_app_options_t){ \
        .size = sizeof(dcc_app_options_t), \
        .client = DCC_CLIENT_OPTIONS((token_), (intents_)), \
        .command_registry = DCC_COMMAND_REGISTRY_OPTIONS_GLOBAL(), \
        .store_file_path = (store_file_path_) \
    })

#define DCC_APP_OPTIONS_GUILD_WITH_STORE(token_, intents_, guild_id_, store_file_path_) \
    ((dcc_app_options_t){ \
        .size = sizeof(dcc_app_options_t), \
        .client = DCC_CLIENT_OPTIONS((token_), (intents_)), \
        .command_registry = DCC_COMMAND_REGISTRY_OPTIONS_GUILD(guild_id_), \
        .store_file_path = (store_file_path_) \
    })

#define DCC_APP_OPTIONS_AUTO_DEFER(token_, intents_, after_ms_) \
    ((dcc_app_options_t){ \
        .size = sizeof(dcc_app_options_t), \
        .client = DCC_CLIENT_OPTIONS((token_), (intents_)), \
        .command_registry = DCC_COMMAND_REGISTRY_OPTIONS_GLOBAL(), \
        .auto_defer_after_ms = (after_ms_) \
    })

#define DCC_APP_OPTIONS_AUTO_DEFER_DEFAULT(token_, intents_) \
    DCC_APP_OPTIONS_AUTO_DEFER((token_), (intents_), 1500U)

#define DCC_APP_OPTIONS_AUTO_DEFER_EPHEMERAL(token_, intents_, after_ms_) \
    ((dcc_app_options_t){ \
        .size = sizeof(dcc_app_options_t), \
        .client = DCC_CLIENT_OPTIONS((token_), (intents_)), \
        .command_registry = DCC_COMMAND_REGISTRY_OPTIONS_GLOBAL(), \
        .auto_defer_after_ms = (after_ms_), \
        .auto_defer_ephemeral = 1U \
    })

#define DCC_APP_OPTIONS_AUTO_DEFER_PRIVATE(token_, intents_, after_ms_) \
    DCC_APP_OPTIONS_AUTO_DEFER_EPHEMERAL((token_), (intents_), (after_ms_))

#define DCC_APP_OPTIONS_AUTO_DEFER_EPHEMERAL_DEFAULT(token_, intents_) \
    DCC_APP_OPTIONS_AUTO_DEFER_EPHEMERAL((token_), (intents_), 1500U)

#define DCC_APP_OPTIONS_AUTO_DEFER_PRIVATE_DEFAULT(token_, intents_) \
    DCC_APP_OPTIONS_AUTO_DEFER_EPHEMERAL_DEFAULT((token_), (intents_))

#define DCC_APP_COMMAND_SYNC(application_id_) \
    ((dcc_app_command_sync_options_t){ \
        .size = sizeof(dcc_app_command_sync_options_t), \
        .application_id = (application_id_), \
        .command_registry = DCC_COMMAND_REGISTRY_OPTIONS_GLOBAL(), \
        .apply = 1U, \
        .once = 1U \
    })

#define DCC_APP_COMMAND_SYNC_PLAN(application_id_) \
    ((dcc_app_command_sync_options_t){ \
        .size = sizeof(dcc_app_command_sync_options_t), \
        .application_id = (application_id_), \
        .command_registry = DCC_COMMAND_REGISTRY_OPTIONS_GLOBAL_DRY_RUN(), \
        .once = 1U \
    })

#define DCC_APP_COMMAND_SYNC_GUILD(application_id_, guild_id_) \
    ((dcc_app_command_sync_options_t){ \
        .size = sizeof(dcc_app_command_sync_options_t), \
        .application_id = (application_id_), \
        .command_registry = DCC_COMMAND_REGISTRY_OPTIONS_GUILD((guild_id_)), \
        .apply = 1U, \
        .once = 1U \
    })

#define DCC_APP_COMMAND_SYNC_GUILD_PLAN(application_id_, guild_id_) \
    ((dcc_app_command_sync_options_t){ \
        .size = sizeof(dcc_app_command_sync_options_t), \
        .application_id = (application_id_), \
        .command_registry = DCC_COMMAND_REGISTRY_OPTIONS_GUILD_DRY_RUN((guild_id_)), \
        .once = 1U \
    })

#define DCC_APP_OPTIONS_WITH_COMMAND_SYNC(token_, intents_, application_id_) \
    ((dcc_app_options_t){ \
        .size = sizeof(dcc_app_options_t), \
        .client = DCC_CLIENT_OPTIONS((token_), (intents_)), \
        .command_registry = DCC_COMMAND_REGISTRY_OPTIONS_GLOBAL(), \
        .command_sync = DCC_APP_COMMAND_SYNC((application_id_)), \
        .command_sync_on_ready = 1U \
    })

#define DCC_APP_OPTIONS_WITH_AUTO_SYNC(token_, intents_) \
    ((dcc_app_options_t){ \
        .size = sizeof(dcc_app_options_t), \
        .client = DCC_CLIENT_OPTIONS((token_), (intents_)), \
        .command_registry = DCC_COMMAND_REGISTRY_OPTIONS_GLOBAL(), \
        .command_sync = DCC_APP_COMMAND_SYNC_AUTO(), \
        .command_sync_on_ready = 1U \
    })

#define DCC_APP_OPTIONS_WITH_AUTO_SYNC_PLAN(token_, intents_) \
    ((dcc_app_options_t){ \
        .size = sizeof(dcc_app_options_t), \
        .client = DCC_CLIENT_OPTIONS((token_), (intents_)), \
        .command_registry = DCC_COMMAND_REGISTRY_OPTIONS_GLOBAL(), \
        .command_sync = DCC_APP_COMMAND_SYNC_AUTO_PLAN(), \
        .command_sync_on_ready = 1U \
    })

#define DCC_APP_OPTIONS_WITH_AUTO_SYNC_STORE(token_, intents_, store_file_path_) \
    ((dcc_app_options_t){ \
        .size = sizeof(dcc_app_options_t), \
        .client = DCC_CLIENT_OPTIONS((token_), (intents_)), \
        .command_registry = DCC_COMMAND_REGISTRY_OPTIONS_GLOBAL(), \
        .command_sync = DCC_APP_COMMAND_SYNC_AUTO(), \
        .command_sync_on_ready = 1U, \
        .store_file_path = (store_file_path_) \
    })

#define DCC_APP_OPTIONS_GUILD_WITH_COMMAND_SYNC(token_, intents_, application_id_, guild_id_) \
    ((dcc_app_options_t){ \
        .size = sizeof(dcc_app_options_t), \
        .client = DCC_CLIENT_OPTIONS((token_), (intents_)), \
        .command_registry = DCC_COMMAND_REGISTRY_OPTIONS_GUILD((guild_id_)), \
        .command_sync = DCC_APP_COMMAND_SYNC_GUILD((application_id_), (guild_id_)), \
        .command_sync_on_ready = 1U \
    })

#define DCC_APP_OPTIONS_GUILD_WITH_AUTO_SYNC(token_, intents_, guild_id_) \
    ((dcc_app_options_t){ \
        .size = sizeof(dcc_app_options_t), \
        .client = DCC_CLIENT_OPTIONS((token_), (intents_)), \
        .command_registry = DCC_COMMAND_REGISTRY_OPTIONS_GUILD((guild_id_)), \
        .command_sync = DCC_APP_COMMAND_SYNC_GUILD_AUTO((guild_id_)), \
        .command_sync_on_ready = 1U \
    })

#define DCC_APP_OPTIONS_GUILD_WITH_AUTO_SYNC_PLAN(token_, intents_, guild_id_) \
    ((dcc_app_options_t){ \
        .size = sizeof(dcc_app_options_t), \
        .client = DCC_CLIENT_OPTIONS((token_), (intents_)), \
        .command_registry = DCC_COMMAND_REGISTRY_OPTIONS_GUILD((guild_id_)), \
        .command_sync = DCC_APP_COMMAND_SYNC_GUILD_AUTO_PLAN((guild_id_)), \
        .command_sync_on_ready = 1U \
    })

#define DCC_APP_OPTIONS_GUILD_WITH_AUTO_SYNC_STORE(token_, intents_, guild_id_, store_file_path_) \
    ((dcc_app_options_t){ \
        .size = sizeof(dcc_app_options_t), \
        .client = DCC_CLIENT_OPTIONS((token_), (intents_)), \
        .command_registry = DCC_COMMAND_REGISTRY_OPTIONS_GUILD((guild_id_)), \
        .command_sync = DCC_APP_COMMAND_SYNC_GUILD_AUTO((guild_id_)), \
        .command_sync_on_ready = 1U, \
        .store_file_path = (store_file_path_) \
    })

#define DCC_APP_STATE(type_, app_) ((type_ *)dcc_app_state((app_)))
#define DCC_CTX_STATE(type_, ctx_) ((type_ *)dcc_ctx_state((ctx_)))
#define DCC_APP_STORE(app_) dcc_app_store((app_))
#define DCC_APP_STORE_FILE(app_, path_) dcc_app_store_open_file((app_), (path_))
#define DCC_APP_STORE_CLOSE(app_) dcc_app_store_close((app_))

#define DCC_APP_ROUTE_GROUP(app_, custom_id_prefix_, user_data_) \
    ((dcc_app_route_group_t){ \
        .size = sizeof(dcc_app_route_group_t), \
        .app = (app_), \
        .custom_id_prefix = (custom_id_prefix_), \
        .user_data = (user_data_) \
    })

#define DCC_EMBED_EMPTY() ((dcc_embed_builder_t){0})
#define DCC_EMBED(title_, description_) \
    ((dcc_embed_builder_t){ \
        .title = (title_), \
        .description = (description_) \
    })
#define DCC_EMBED_COLOR(title_, description_, color_) \
    ((dcc_embed_builder_t){ \
        .title = (title_), \
        .description = (description_), \
        .color = (color_), \
        .has_color = 1U \
    })
#define DCC_EMBED_URL(title_, description_, url_) \
    ((dcc_embed_builder_t){ \
        .title = (title_), \
        .description = (description_), \
        .url = (url_) \
    })
#define DCC_EMBED_FIELD(name_, value_) \
    ((dcc_embed_field_t){ \
        .name = (name_), \
        .value = (value_) \
    })
#define DCC_EMBED_FIELD_INLINE(name_, value_) \
    ((dcc_embed_field_t){ \
        .name = (name_), \
        .value = (value_), \
        .inline_field = 1U, \
        .has_inline = 1U \
    })
#define DCC_EMBED_FOOTER(text_, icon_url_) \
    ((dcc_embed_footer_t){ \
        .text = (text_), \
        .icon_url = (icon_url_) \
    })
#define DCC_EMBED_IMAGE(url_) ((dcc_embed_media_t){ .url = (url_) })
#define DCC_EMBED_AUTHOR(name_, url_, icon_url_) \
    ((dcc_embed_author_t){ \
        .name = (name_), \
        .url = (url_), \
        .icon_url = (icon_url_) \
    })
#define DCC_EMBED_WITH_FIELDS_ARRAY(title_, description_, fields_, field_count_) \
    ((dcc_embed_builder_t){ \
        .title = (title_), \
        .description = (description_), \
        .fields = (fields_), \
        .field_count = (field_count_) \
    })
#define DCC_EMBED_WITH_FIELDS(title_, description_, ...) \
    ((dcc_embed_builder_t){ \
        .title = (title_), \
        .description = (description_), \
        .fields = DCC_SUGAR_ARRAY(dcc_embed_field_t, __VA_ARGS__), \
        .field_count = DCC_SUGAR_ARRAY_LEN(dcc_embed_field_t, __VA_ARGS__) \
    })

#define DCC_MESSAGE_EMPTY() ((dcc_message_builder_t){0})
#define DCC_MESSAGE_TEXT(content_) \
    ((dcc_message_builder_t){ \
        .content = (content_), \
        .has_content = 1U \
    })
#define DCC_MESSAGE_TTS(content_) \
    ((dcc_message_builder_t){ \
        .content = (content_), \
        .tts = 1U, \
        .has_content = 1U, \
        .has_tts = 1U \
    })
#define DCC_MESSAGE_EPHEMERAL(content_) \
    ((dcc_message_builder_t){ \
        .content = (content_), \
        .flags = DCC_MESSAGE_FLAG_EPHEMERAL, \
        .has_content = 1U, \
        .has_flags = 1U \
    })
#define DCC_MESSAGE_SUPPRESS_EMBEDS(content_) \
    ((dcc_message_builder_t){ \
        .content = (content_), \
        .flags = DCC_MESSAGE_FLAG_SUPPRESS_EMBEDS, \
        .has_content = 1U, \
        .has_flags = 1U \
    })
#define DCC_MESSAGE_EMBEDS_ARRAY(embeds_, embed_count_) \
    ((dcc_message_builder_t){ \
        .embeds = (embeds_), \
        .embeds_count = (embed_count_) \
    })
#define DCC_MESSAGE_EMBEDS(...) \
    ((dcc_message_builder_t){ \
        .embeds = DCC_SUGAR_ARRAY(dcc_embed_builder_t, __VA_ARGS__), \
        .embeds_count = DCC_SUGAR_ARRAY_LEN(dcc_embed_builder_t, __VA_ARGS__) \
    })
#define DCC_MESSAGE_TEXT_EMBEDS_ARRAY(content_, embeds_, embed_count_) \
    ((dcc_message_builder_t){ \
        .content = (content_), \
        .embeds = (embeds_), \
        .embeds_count = (embed_count_), \
        .has_content = 1U \
    })
#define DCC_MESSAGE_TEXT_EMBEDS(content_, ...) \
    ((dcc_message_builder_t){ \
        .content = (content_), \
        .embeds = DCC_SUGAR_ARRAY(dcc_embed_builder_t, __VA_ARGS__), \
        .embeds_count = DCC_SUGAR_ARRAY_LEN(dcc_embed_builder_t, __VA_ARGS__), \
        .has_content = 1U \
    })
#define DCC_MESSAGE_COMPONENTS_ARRAY(components_, component_count_) \
    ((dcc_message_builder_t){ \
        .components = (components_), \
        .components_count = (component_count_) \
    })
#define DCC_MESSAGE_COMPONENTS(...) \
    ((dcc_message_builder_t){ \
        .components = DCC_SUGAR_ARRAY(dcc_component_builder_t, __VA_ARGS__), \
        .components_count = DCC_SUGAR_ARRAY_LEN(dcc_component_builder_t, __VA_ARGS__) \
    })
#define DCC_MESSAGE_COMPONENTS_JSON(components_json_) \
    ((dcc_message_builder_t){ \
        .components_json = (components_json_) \
    })
#define DCC_MESSAGE_COMPONENTS_V2_ARRAY(components_, component_count_) \
    ((dcc_message_builder_t){ \
        .components_v2 = (components_), \
        .components_v2_count = (component_count_), \
        .flags = DCC_MESSAGE_FLAG_IS_COMPONENTS_V2, \
        .has_flags = 1U \
    })
#define DCC_MESSAGE_COMPONENTS_V2(...) \
    ((dcc_message_builder_t){ \
        .components_v2 = DCC_SUGAR_ARRAY(dcc_component_v2_builder_t, __VA_ARGS__), \
        .components_v2_count = DCC_SUGAR_ARRAY_LEN(dcc_component_v2_builder_t, __VA_ARGS__), \
        .flags = DCC_MESSAGE_FLAG_IS_COMPONENTS_V2, \
        .has_flags = 1U \
    })
#define DCC_MESSAGE_COMPONENTS_V2_FLAGS(flags_, ...) \
    ((dcc_message_builder_t){ \
        .components_v2 = DCC_SUGAR_ARRAY(dcc_component_v2_builder_t, __VA_ARGS__), \
        .components_v2_count = DCC_SUGAR_ARRAY_LEN(dcc_component_v2_builder_t, __VA_ARGS__), \
        .flags = ((flags_) | DCC_MESSAGE_FLAG_IS_COMPONENTS_V2), \
        .has_flags = 1U \
    })
#define DCC_MESSAGE_EPHEMERAL_COMPONENTS_V2(...) \
    DCC_MESSAGE_COMPONENTS_V2_FLAGS(DCC_MESSAGE_FLAG_EPHEMERAL, __VA_ARGS__)
#define DCC_MESSAGE_COMPONENTS_V2_JSON(components_json_) \
    ((dcc_message_builder_t){ \
        .components_v2_json = (components_json_), \
        .flags = DCC_MESSAGE_FLAG_IS_COMPONENTS_V2, \
        .has_flags = 1U \
    })
#define DCC_MESSAGE_COMPONENTS_V2_JSON_FLAGS(flags_, components_json_) \
    ((dcc_message_builder_t){ \
        .components_v2_json = (components_json_), \
        .flags = ((flags_) | DCC_MESSAGE_FLAG_IS_COMPONENTS_V2), \
        .has_flags = 1U \
    })
#define DCC_MESSAGE_EPHEMERAL_COMPONENTS_V2_JSON(components_json_) \
    DCC_MESSAGE_COMPONENTS_V2_JSON_FLAGS(DCC_MESSAGE_FLAG_EPHEMERAL, (components_json_))
#define DCC_REPLY_COMPONENTS_V2(ctx_, ...) \
    DCC_REPLY((ctx_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__))
#define DCC_REPLY_V2(ctx_, ...) DCC_REPLY_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_COMPONENTS_V2(ctx_, ...) \
    DCC_REPLY_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_V2(ctx_, ...) DCC_RESPOND_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_EDIT_COMPONENTS_V2(ctx_, ...) \
    DCC_RESPOND_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_EDIT_V2(ctx_, ...) \
    DCC_RESPOND_OR_EDIT_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_FOLLOWUP_COMPONENTS_V2(ctx_, ...) \
    DCC_RESPOND_OR_EDIT_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_FOLLOWUP_V2(ctx_, ...) \
    DCC_RESPOND_OR_FOLLOWUP_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_REPLY_COMPONENTS_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_REPLY_CB((ctx_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__), (cb_), (user_data_))
#define DCC_REPLY_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_REPLY_COMPONENTS_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_RESPOND_COMPONENTS_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_REPLY_COMPONENTS_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_RESPOND_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_RESPOND_COMPONENTS_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_RESPOND_OR_EDIT_COMPONENTS_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_RESPOND_COMPONENTS_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_RESPOND_OR_EDIT_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_RESPOND_OR_EDIT_COMPONENTS_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_RESPOND_OR_FOLLOWUP_COMPONENTS_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_RESPOND_OR_EDIT_COMPONENTS_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_RESPOND_OR_FOLLOWUP_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_RESPOND_OR_FOLLOWUP_COMPONENTS_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_REPLY_EPHEMERAL_COMPONENTS_V2(ctx_, ...) \
    DCC_REPLY((ctx_), DCC_MESSAGE_EPHEMERAL_COMPONENTS_V2(__VA_ARGS__))
#define DCC_REPLY_EPHEMERAL_V2(ctx_, ...) DCC_REPLY_EPHEMERAL_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_EPHEMERAL_COMPONENTS_V2(ctx_, ...) \
    DCC_REPLY_EPHEMERAL_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_EPHEMERAL_V2(ctx_, ...) \
    DCC_RESPOND_EPHEMERAL_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_PRIVATE_V2(ctx_, ...) DCC_RESPOND_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_EDIT_EPHEMERAL_COMPONENTS_V2(ctx_, ...) \
    DCC_RESPOND_EPHEMERAL_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_EDIT_EPHEMERAL_V2(ctx_, ...) \
    DCC_RESPOND_OR_EDIT_EPHEMERAL_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_EDIT_PRIVATE_V2(ctx_, ...) \
    DCC_RESPOND_OR_EDIT_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_FOLLOWUP_EPHEMERAL_COMPONENTS_V2(ctx_, ...) \
    DCC_RESPOND_OR_EDIT_EPHEMERAL_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_FOLLOWUP_EPHEMERAL_V2(ctx_, ...) \
    DCC_RESPOND_OR_FOLLOWUP_EPHEMERAL_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_FOLLOWUP_PRIVATE_V2(ctx_, ...) \
    DCC_RESPOND_OR_FOLLOWUP_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_REPLY_V2(ctx_, ...) DCC_REPLY_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_REPLY_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_REPLY_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_EPHEMERAL_V2(ctx_, ...) DCC_REPLY_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_V2(ctx_, ...) DCC_RESPOND_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_RESPOND_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_EDIT_V2(ctx_, ...) DCC_RESPOND_OR_EDIT_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_EDIT_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_RESPOND_OR_EDIT_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_FOLLOWUP_V2(ctx_, ...) DCC_RESPOND_OR_FOLLOWUP_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_FOLLOWUP_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_RESPOND_OR_FOLLOWUP_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_RESPOND_EPHEMERAL_V2(ctx_, ...) \
    DCC_RESPOND_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_PRIVATE_V2(ctx_, ...) DCC_RESPOND_PRIVATE_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_EDIT_EPHEMERAL_V2(ctx_, ...) \
    DCC_RESPOND_OR_EDIT_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_EDIT_PRIVATE_V2(ctx_, ...) \
    DCC_RESPOND_OR_EDIT_PRIVATE_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_FOLLOWUP_EPHEMERAL_V2(ctx_, ...) \
    DCC_RESPOND_OR_FOLLOWUP_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_FOLLOWUP_PRIVATE_V2(ctx_, ...) \
    DCC_RESPOND_OR_FOLLOWUP_PRIVATE_V2((ctx_), __VA_ARGS__)
#define DCC_UPDATE_COMPONENTS_V2(ctx_, ...) \
    DCC_UPDATE((ctx_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__))
#define DCC_UPDATE_V2(ctx_, ...) DCC_UPDATE_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_UPDATE_COMPONENTS_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_UPDATE_CB((ctx_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__), (cb_), (user_data_))
#define DCC_UPDATE_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_UPDATE_COMPONENTS_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_UPDATE_V2(ctx_, ...) DCC_UPDATE_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_UPDATE_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_UPDATE_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_FOLLOWUP_COMPONENTS_V2(ctx_, ...) \
    DCC_FOLLOWUP((ctx_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__))
#define DCC_FOLLOWUP_V2(ctx_, ...) DCC_FOLLOWUP_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_FOLLOWUP_COMPONENTS_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_FOLLOWUP_CB((ctx_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__), (cb_), (user_data_))
#define DCC_FOLLOWUP_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_FOLLOWUP_COMPONENTS_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_FOLLOWUP_EPHEMERAL_COMPONENTS_V2(ctx_, ...) \
    DCC_FOLLOWUP((ctx_), DCC_MESSAGE_EPHEMERAL_COMPONENTS_V2(__VA_ARGS__))
#define DCC_FOLLOWUP_EPHEMERAL_V2(ctx_, ...) \
    DCC_FOLLOWUP_EPHEMERAL_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_FOLLOWUP_V2(ctx_, ...) DCC_FOLLOWUP_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_FOLLOWUP_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_FOLLOWUP_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_FOLLOWUP_EPHEMERAL_V2(ctx_, ...) \
    DCC_FOLLOWUP_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_SEND_COMPONENTS_V2(ctx_, ...) \
    DCC_SEND((ctx_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__))
#define DCC_SEND_V2(ctx_, ...) DCC_SEND_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_SEND_V2_WITH_ID(ctx_, cb_, user_data_, ...) \
    DCC_SEND_WITH_ID((ctx_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__), (cb_), (user_data_))
#define DCC_SEND_V2_ID(ctx_, cb_, user_data_, ...) \
    DCC_SEND_V2_WITH_ID((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_SEND_V2(ctx_, ...) DCC_SEND_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_SEND_V2_WITH_ID(ctx_, cb_, user_data_, ...) \
    DCC_SEND_V2_WITH_ID((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_SEND_V2_ID(ctx_, cb_, user_data_, ...) \
    DCC_CTX_SEND_V2_WITH_ID((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_MANAGED_MESSAGE_OPTIONS(channel_id_, message_, load_, save_, storage_user_data_) \
    ((dcc_managed_message_options_t){ \
        .size = sizeof(dcc_managed_message_options_t), \
        .channel_id = (channel_id_), \
        .message = (message_), \
        .load = (load_), \
        .save = (save_), \
        .storage_user_data = (storage_user_data_) \
    })
#define DCC_MANAGED_MESSAGE_KEEP_PREVIOUS_OPTIONS(channel_id_, message_, load_, save_, storage_user_data_) \
    ((dcc_managed_message_options_t){ \
        .size = sizeof(dcc_managed_message_options_t), \
        .channel_id = (channel_id_), \
        .message = (message_), \
        .load = (load_), \
        .save = (save_), \
        .storage_user_data = (storage_user_data_), \
        .keep_previous = 1U \
    })
#define DCC_STORE_FILE(path_, out_store_) dcc_store_file_open((path_), (out_store_))
#define DCC_STORE_FILE_OPTIONS(path_) \
    ((dcc_store_file_options_t){ \
        .size = sizeof(dcc_store_file_options_t), \
        .path = (path_) \
    })
#define DCC_STORE_MANAGED_MESSAGE(key_, store_) \
    ((dcc_store_managed_message_binding_t){ \
        .size = sizeof(dcc_store_managed_message_binding_t), \
        .store = (store_), \
        .key = (key_) \
    })
#define DCC_APP_STORE_MANAGED_MESSAGE(app_, key_) \
    DCC_STORE_MANAGED_MESSAGE((key_), DCC_APP_STORE((app_)))
#define DCC_STORE_GET_U64(store_, key_, out_) dcc_store_get_u64((store_), (key_), (out_))
#define DCC_STORE_SET_U64(store_, key_, value_) dcc_store_set_u64((store_), (key_), (value_))
#define DCC_MANAGED_MESSAGE_STORE_OPTIONS(channel_id_, message_, binding_) \
    DCC_MANAGED_MESSAGE_OPTIONS( \
        (channel_id_), \
        (message_), \
        dcc_store_load_managed_message_ref, \
        dcc_store_save_managed_message_ref, \
        (binding_) \
    )
#define DCC_MANAGED_MESSAGE_STORE_KEEP_PREVIOUS_OPTIONS(channel_id_, message_, binding_) \
    DCC_MANAGED_MESSAGE_KEEP_PREVIOUS_OPTIONS( \
        (channel_id_), \
        (message_), \
        dcc_store_load_managed_message_ref, \
        dcc_store_save_managed_message_ref, \
        (binding_) \
    )
#define DCC_APP_PUBLISH_LATEST(app_, options_, cb_, user_data_) \
    dcc_app_managed_message_publish_latest((app_), (options_), (cb_), (user_data_))
#define DCC_APP_PUBLISH_LATEST_STORE(app_, key_, channel_id_, message_, cb_, user_data_) \
    dcc_app_managed_message_publish_latest_store((app_), (key_), (channel_id_), (message_), (cb_), (user_data_))
#define DCC_APP_PUBLISH_LATEST_STORE_BUILDER(app_, key_, channel_id_, message_, cb_, user_data_) \
    DCC_APP_PUBLISH_LATEST_STORE( \
        (app_), \
        (key_), \
        (channel_id_), \
        DCC_SUGAR_PTR(dcc_message_builder_t, (message_)), \
        (cb_), \
        (user_data_) \
    )
#define DCC_APP_PUBLISH_LATEST_STORE_TEXT(app_, key_, channel_id_, content_, cb_, user_data_) \
    DCC_APP_PUBLISH_LATEST_STORE_BUILDER( \
        (app_), \
        (key_), \
        (channel_id_), \
        DCC_MESSAGE_TEXT((content_)), \
        (cb_), \
        (user_data_) \
    )
#define DCC_APP_PUBLISH_LATEST_STORE_V2(app_, key_, channel_id_, cb_, user_data_, ...) \
    DCC_APP_PUBLISH_LATEST_STORE_BUILDER( \
        (app_), \
        (key_), \
        (channel_id_), \
        DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__), \
        (cb_), \
        (user_data_) \
    )
#define DCC_APP_PUBLISH_LATEST_STORE_UI(app_, key_, channel_id_, cb_, user_data_, ...) \
    DCC_APP_PUBLISH_LATEST_STORE_V2((app_), (key_), (channel_id_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_PUBLISH_LATEST(client_, options_, cb_, user_data_) \
    dcc_managed_message_publish_latest((client_), (options_), (cb_), (user_data_))
#define DCC_APP_ON_READY(app_, handler_) dcc_app_on_ready((app_), (handler_), NULL)
#define DCC_APP_ON_READY_DATA(app_, handler_, user_data_) \
    dcc_app_on_ready((app_), (handler_), (user_data_))
#define DCC_APP_ON_EVENT(app_, type_, handler_) dcc_app_on((app_), (type_), (handler_), NULL)
#define DCC_APP_ON_EVENT_DATA(app_, type_, handler_, user_data_) \
    dcc_app_on((app_), (type_), (handler_), (user_data_))
#define DCC_APP_ON_MESSAGE(app_, handler_) dcc_app_on_message_create((app_), (handler_), NULL)
#define DCC_APP_ON_MESSAGE_DATA(app_, handler_, user_data_) \
    dcc_app_on_message_create((app_), (handler_), (user_data_))
#define DCC_APP_ON_MESSAGE_CREATE(app_, handler_) \
    dcc_app_on_message_create((app_), (handler_), NULL)
#define DCC_APP_ON_MESSAGE_CREATE_DATA(app_, handler_, user_data_) \
    dcc_app_on_message_create((app_), (handler_), (user_data_))
#define DCC_APP_ON_MESSAGE_UPDATE(app_, handler_) \
    dcc_app_on_message_update((app_), (handler_), NULL)
#define DCC_APP_ON_MESSAGE_UPDATE_DATA(app_, handler_, user_data_) \
    dcc_app_on_message_update((app_), (handler_), (user_data_))
#define DCC_APP_ON_MESSAGE_DELETE(app_, handler_) \
    dcc_app_on_message_delete((app_), (handler_), NULL)
#define DCC_APP_ON_MESSAGE_DELETE_DATA(app_, handler_, user_data_) \
    dcc_app_on_message_delete((app_), (handler_), (user_data_))
#define DCC_APP_ON_MESSAGE_COMMAND(app_, prefix_, name_, handler_) \
    dcc_app_on_message_command((app_), (prefix_), (name_), (handler_), NULL)
#define DCC_APP_ON_MESSAGE_COMMAND_DATA(app_, prefix_, name_, handler_, user_data_) \
    dcc_app_on_message_command((app_), (prefix_), (name_), (handler_), (user_data_))
#define DCC_APP_ON_PREFIX_COMMAND(app_, prefix_, name_, handler_) \
    DCC_APP_ON_MESSAGE_COMMAND((app_), (prefix_), (name_), (handler_))
#define DCC_APP_ON_PREFIX_COMMAND_DATA(app_, prefix_, name_, handler_, user_data_) \
    DCC_APP_ON_MESSAGE_COMMAND_DATA((app_), (prefix_), (name_), (handler_), (user_data_))
#define DCC_APP_ON_TEXT_COMMAND(app_, prefix_, name_, handler_) \
    DCC_APP_ON_PREFIX_COMMAND((app_), (prefix_), (name_), (handler_))
#define DCC_APP_ON_TEXT_COMMAND_DATA(app_, prefix_, name_, handler_, user_data_) \
    DCC_APP_ON_PREFIX_COMMAND_DATA((app_), (prefix_), (name_), (handler_), (user_data_))
#define DCC_APP_ON_MESSAGE_COMMAND_FN(app_, prefix_, handler_) \
    DCC_APP_ON_MESSAGE_COMMAND((app_), (prefix_), #handler_, (handler_))
#define DCC_APP_ON_MESSAGE_COMMAND_FN_DATA(app_, prefix_, handler_, user_data_) \
    DCC_APP_ON_MESSAGE_COMMAND_DATA((app_), (prefix_), #handler_, (handler_), (user_data_))
#define DCC_APP_ON_PREFIX_COMMAND_FN(app_, prefix_, handler_) \
    DCC_APP_ON_MESSAGE_COMMAND_FN((app_), (prefix_), handler_)
#define DCC_APP_ON_PREFIX_COMMAND_FN_DATA(app_, prefix_, handler_, user_data_) \
    DCC_APP_ON_MESSAGE_COMMAND_FN_DATA((app_), (prefix_), handler_, (user_data_))
#define DCC_APP_ON_TEXT_COMMAND_FN(app_, prefix_, handler_) \
    DCC_APP_ON_PREFIX_COMMAND_FN((app_), (prefix_), handler_)
#define DCC_APP_ON_TEXT_COMMAND_FN_DATA(app_, prefix_, handler_, user_data_) \
    DCC_APP_ON_PREFIX_COMMAND_FN_DATA((app_), (prefix_), handler_, (user_data_))
