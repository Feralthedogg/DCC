/* Included by <dcc/sugar.h>. */
#define DCC_ME_EDIT_USER_PARAMS(app_, params_) \
    DCC_APP_EDIT_CURRENT_USER_PARAMS((app_), (params_))
#define DCC_ME_CONNECTIONS_FETCH(app_) \
    DCC_APP_GET_CURRENT_USER_CONNECTIONS((app_))
#define DCC_ME_GUILDS_FETCH(app_) \
    DCC_APP_GET_CURRENT_USER_GUILDS((app_))
#define DCC_ME_GUILDS_FETCH_QUERY(app_, query_) \
    DCC_APP_GET_CURRENT_USER_GUILDS_QUERY((app_), (query_))
#define DCC_ME_DMS_FETCH(app_) \
    DCC_APP_GET_CURRENT_USER_DMS((app_))
#define DCC_USER_FETCH(app_, user_id_) \
    DCC_APP_GET_USER((app_), (user_id_))
#define DCC_USER_DM_CHANNEL_CREATE(app_, user_id_) \
    DCC_APP_CREATE_DM_CHANNEL_PARAMS( \
        (app_), \
        &((dcc_dm_channel_params_t[]){ { \
            .size = sizeof(dcc_dm_channel_params_t), \
            .user_id = (user_id_) \
        } })[0] \
    )
#define DCC_GROUP_DM_RECIPIENT_ADD(app_, channel_id_, user_id_, json_body_) \
    DCC_APP_ADD_GROUP_DM_RECIPIENT((app_), (channel_id_), (user_id_), (json_body_))
#define DCC_GROUP_DM_RECIPIENT_ADD_PARAMS(app_, params_) \
    DCC_APP_ADD_GROUP_DM_RECIPIENT_PARAMS((app_), (params_))
#define DCC_GROUP_DM_RECIPIENT_REMOVE(app_, channel_id_, user_id_) \
    DCC_APP_REMOVE_GROUP_DM_RECIPIENT((app_), (channel_id_), (user_id_))
#define DCC_USER_DM_SEND_JSON(app_, user_id_, json_body_) \
    DCC_APP_CREATE_DIRECT_MESSAGE((app_), (user_id_), (json_body_))
#define DCC_USER_DM_SEND(app_, user_id_, message_) \
    DCC_APP_CREATE_DIRECT_MESSAGE_BUILDER((app_), (user_id_), (message_))
#define DCC_USER_DM_SEND_TEXT(app_, user_id_, content_) \
    DCC_APP_CREATE_DIRECT_MESSAGE_TEXT((app_), (user_id_), (content_))
#define DCC_USER_DM_SEND_UI(app_, user_id_, ...) \
    DCC_USER_DM_SEND((app_), (user_id_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__))
#define DCC_DM_SEND(app_, user_id_, message_) \
    DCC_USER_DM_SEND((app_), (user_id_), (message_))
#define DCC_DM_SEND_TEXT(app_, user_id_, content_) \
    DCC_USER_DM_SEND_TEXT((app_), (user_id_), (content_))
#define DCC_DM_SEND_UI(app_, user_id_, ...) \
    DCC_USER_DM_SEND_UI((app_), (user_id_), __VA_ARGS__)
#define DCC_GUILD_LEAVE(app_, guild_id_) \
    DCC_APP_LEAVE_GUILD((app_), (guild_id_))
#define DCC_GUILD_INVITES_FETCH(app_, guild_id_) \
    DCC_APP_GET_GUILD_INVITES((app_), (guild_id_))
#define DCC_INVITE_FETCH(app_, invite_code_) \
    DCC_APP_GET_INVITE((app_), (invite_code_))
#define DCC_INVITE_FETCH_QUERY(app_, invite_code_, query_) \
    DCC_APP_GET_INVITE_QUERY((app_), (invite_code_), (query_))
#define DCC_INVITE_FETCH_FULL(app_, invite_code_) \
    DCC_APP_GET_INVITE_FULL((app_), (invite_code_))
#define DCC_INVITE_DELETE(app_, invite_code_) \
    DCC_APP_DELETE_INVITE((app_), (invite_code_))
#define DCC_STAGE_CREATE(app_, json_body_) \
    DCC_APP_CREATE_STAGE((app_), (json_body_))
#define DCC_STAGE_CREATE_PARAMS(app_, params_) \
    DCC_APP_CREATE_STAGE_PARAMS((app_), (params_))
#define DCC_STAGE_FETCH(app_, channel_id_) \
    DCC_APP_GET_STAGE((app_), (channel_id_))
#define DCC_STAGE_EDIT(app_, channel_id_, json_body_) \
    DCC_APP_EDIT_STAGE((app_), (channel_id_), (json_body_))
#define DCC_STAGE_EDIT_PARAMS(app_, params_) \
    DCC_APP_EDIT_STAGE_PARAMS((app_), (params_))
#define DCC_STAGE_DELETE(app_, channel_id_) \
    DCC_APP_DELETE_STAGE((app_), (channel_id_))
#define DCC_TEMPLATE_FETCH(app_, code_) \
    DCC_APP_GET_TEMPLATE((app_), (code_))
#define DCC_TEMPLATE_CREATE_GUILD(app_, code_, json_body_) \
    DCC_APP_CREATE_GUILD_FROM_TEMPLATE((app_), (code_), (json_body_))
#define DCC_TEMPLATE_CREATE_GUILD_PARAMS(app_, params_) \
    DCC_APP_CREATE_GUILD_FROM_TEMPLATE_PARAMS((app_), (params_))
#define DCC_GUILD_TEMPLATES_FETCH(app_, guild_id_) \
    DCC_APP_GET_GUILD_TEMPLATES((app_), (guild_id_))
#define DCC_GUILD_TEMPLATE_CREATE(app_, guild_id_, json_body_) \
    DCC_APP_CREATE_GUILD_TEMPLATE((app_), (guild_id_), (json_body_))
#define DCC_GUILD_TEMPLATE_CREATE_PARAMS(app_, params_) \
    DCC_APP_CREATE_GUILD_TEMPLATE_PARAMS((app_), (params_))
#define DCC_GUILD_TEMPLATE_EDIT(app_, guild_id_, code_, json_body_) \
    DCC_APP_EDIT_GUILD_TEMPLATE((app_), (guild_id_), (code_), (json_body_))
#define DCC_GUILD_TEMPLATE_EDIT_PARAMS(app_, params_) \
    DCC_APP_EDIT_GUILD_TEMPLATE_PARAMS((app_), (params_))
#define DCC_GUILD_TEMPLATE_SYNC(app_, guild_id_, code_) \
    DCC_APP_SYNC_GUILD_TEMPLATE((app_), (guild_id_), (code_))
#define DCC_GUILD_TEMPLATE_DELETE(app_, guild_id_, code_) \
    DCC_APP_DELETE_GUILD_TEMPLATE((app_), (guild_id_), (code_))
#define DCC_MESSAGE_ALLOWED_MENTIONS_JSON(content_, allowed_mentions_json_) \
    ((dcc_message_builder_t){ \
        .content = (content_), \
        .allowed_mentions_json = (allowed_mentions_json_), \
        .has_content = 1U \
    })
#define DCC_MESSAGE_STICKERS_ARRAY(sticker_ids_, sticker_count_) \
    ((dcc_message_builder_t){ \
        .sticker_ids = (sticker_ids_), \
        .sticker_ids_count = (sticker_count_) \
    })
#define DCC_MESSAGE_STICKERS(...) \
    ((dcc_message_builder_t){ \
        .sticker_ids = DCC_SUGAR_ARRAY(dcc_snowflake_t, __VA_ARGS__), \
        .sticker_ids_count = DCC_SUGAR_ARRAY_LEN(dcc_snowflake_t, __VA_ARGS__) \
    })
#define DCC_COMPONENT_EMOJI_NAME(name_) \
    ((dcc_component_emoji_t){ \
        .name = (name_) \
    })
#define DCC_COMPONENT_EMOJI_ID(name_, id_, animated_) \
    ((dcc_component_emoji_t){ \
        .name = (name_), \
        .id = (id_), \
        .animated = (animated_), \
        .has_id = 1U, \
        .has_animated = 1U \
    })
#define DCC_SELECT_OPTION(label_, value_) \
    ((dcc_select_option_t){ \
        .label = (label_), \
        .value = (value_) \
    })
#define DCC_SELECT_OPTION_DESC(label_, value_, description_) \
    ((dcc_select_option_t){ \
        .label = (label_), \
        .value = (value_), \
        .description = (description_) \
    })
#define DCC_SELECT_OPTION_DEFAULT(label_, value_) \
    ((dcc_select_option_t){ \
        .label = (label_), \
        .value = (value_), \
        .is_default = 1U, \
        .has_default = 1U \
    })
#define DCC_SELECT_OPTION_EMOJI(label_, value_, emoji_) \
    ((dcc_select_option_t){ \
        .label = (label_), \
        .value = (value_), \
        .emoji = (emoji_), \
        .has_emoji = 1U \
    })

#define DCC_ACTION_ROW_ARRAY(children_, child_count_) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_ACTION_ROW, \
        .children = (children_), \
        .children_count = (child_count_) \
    })
#define DCC_ACTION_ROW(...) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_ACTION_ROW, \
        .children = DCC_SUGAR_ARRAY(dcc_component_builder_t, __VA_ARGS__), \
        .children_count = DCC_SUGAR_ARRAY_LEN(dcc_component_builder_t, __VA_ARGS__) \
    })
#define DCC_BUTTON_BUILDER(style_, label_, custom_id_) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_BUTTON, \
        .style = (style_), \
        .custom_id = (custom_id_), \
        .label = (label_), \
        .has_style = 1U \
    })
#define DCC_BUTTON_BUILDER_NS(style_, label_, namespace_, action_) \
    DCC_BUTTON_BUILDER((style_), (label_), DCC_COMPONENT_ID(namespace_, action_))
#define DCC_BUTTON_PRIMARY_BUILDER(label_, custom_id_) \
    DCC_BUTTON_BUILDER(DCC_BUTTON_PRIMARY, (label_), (custom_id_))
#define DCC_BUTTON_PRIMARY_BUILDER_NS(label_, namespace_, action_) \
    DCC_BUTTON_BUILDER_NS(DCC_BUTTON_PRIMARY, (label_), namespace_, action_)
#define DCC_BUTTON_SECONDARY_BUILDER(label_, custom_id_) \
    DCC_BUTTON_BUILDER(DCC_BUTTON_SECONDARY, (label_), (custom_id_))
#define DCC_BUTTON_SECONDARY_BUILDER_NS(label_, namespace_, action_) \
    DCC_BUTTON_BUILDER_NS(DCC_BUTTON_SECONDARY, (label_), namespace_, action_)
#define DCC_BUTTON_SUCCESS_BUILDER(label_, custom_id_) \
    DCC_BUTTON_BUILDER(DCC_BUTTON_SUCCESS, (label_), (custom_id_))
#define DCC_BUTTON_SUCCESS_BUILDER_NS(label_, namespace_, action_) \
    DCC_BUTTON_BUILDER_NS(DCC_BUTTON_SUCCESS, (label_), namespace_, action_)
#define DCC_BUTTON_DANGER_BUILDER(label_, custom_id_) \
    DCC_BUTTON_BUILDER(DCC_BUTTON_DANGER, (label_), (custom_id_))
#define DCC_BUTTON_DANGER_BUILDER_NS(label_, namespace_, action_) \
    DCC_BUTTON_BUILDER_NS(DCC_BUTTON_DANGER, (label_), namespace_, action_)
#define DCC_LINK_BUTTON_BUILDER(label_, url_) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_BUTTON, \
        .style = DCC_BUTTON_LINK, \
        .label = (label_), \
        .url = (url_), \
        .has_style = 1U \
    })
#define DCC_PREMIUM_BUTTON_BUILDER(sku_id_) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_BUTTON, \
        .style = DCC_BUTTON_PREMIUM, \
        .sku_id = (sku_id_), \
        .has_style = 1U, \
        .has_sku_id = 1U \
    })
#define DCC_STRING_SELECT_BUILDER_ARRAY(custom_id_, options_, option_count_) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_STRING_SELECT, \
        .custom_id = (custom_id_), \
        .options = (options_), \
        .options_count = (option_count_) \
    })
#define DCC_STRING_SELECT_BUILDER(custom_id_, ...) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_STRING_SELECT, \
        .custom_id = (custom_id_), \
        .options = DCC_SUGAR_ARRAY(dcc_select_option_t, __VA_ARGS__), \
        .options_count = DCC_SUGAR_ARRAY_LEN(dcc_select_option_t, __VA_ARGS__) \
    })
#define DCC_STRING_SELECT_BUILDER_NS(namespace_, action_, ...) \
    DCC_STRING_SELECT_BUILDER(DCC_COMPONENT_ID(namespace_, action_), __VA_ARGS__)
#define DCC_USER_SELECT_BUILDER(custom_id_) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_USER_SELECT, \
        .custom_id = (custom_id_) \
    })
#define DCC_USER_SELECT_BUILDER_NS(namespace_, action_) \
    DCC_USER_SELECT_BUILDER(DCC_COMPONENT_ID(namespace_, action_))
#define DCC_ROLE_SELECT_BUILDER(custom_id_) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_ROLE_SELECT, \
        .custom_id = (custom_id_) \
    })
#define DCC_ROLE_SELECT_BUILDER_NS(namespace_, action_) \
    DCC_ROLE_SELECT_BUILDER(DCC_COMPONENT_ID(namespace_, action_))
#define DCC_MENTIONABLE_SELECT_BUILDER(custom_id_) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_MENTIONABLE_SELECT, \
        .custom_id = (custom_id_) \
    })
#define DCC_MENTIONABLE_SELECT_BUILDER_NS(namespace_, action_) \
    DCC_MENTIONABLE_SELECT_BUILDER(DCC_COMPONENT_ID(namespace_, action_))
#define DCC_CHANNEL_SELECT_BUILDER(custom_id_) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_CHANNEL_SELECT, \
        .custom_id = (custom_id_) \
    })
#define DCC_CHANNEL_SELECT_BUILDER_NS(namespace_, action_) \
    DCC_CHANNEL_SELECT_BUILDER(DCC_COMPONENT_ID(namespace_, action_))
#define DCC_TEXT_INPUT_BUILDER(custom_id_, label_, style_) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_TEXT_INPUT, \
        .custom_id = (custom_id_), \
        .label = (label_), \
        .text_input_style = (style_), \
        .has_text_input_style = 1U \
    })
#define DCC_TEXT_INPUT_BUILDER_NS(namespace_, action_, label_, style_) \
    DCC_TEXT_INPUT_BUILDER(DCC_COMPONENT_ID(namespace_, action_), (label_), (style_))
#define DCC_TEXT_INPUT_REQUIRED(custom_id_, label_, style_, required_) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_TEXT_INPUT, \
        .custom_id = (custom_id_), \
        .label = (label_), \
        .text_input_style = (style_), \
        .required = (required_), \
        .has_text_input_style = 1U, \
        .has_required = 1U \
    })
#define DCC_TEXT_INPUT_REQUIRED_NS(namespace_, action_, label_, style_, required_) \
    DCC_TEXT_INPUT_REQUIRED(DCC_COMPONENT_ID(namespace_, action_), (label_), (style_), (required_))
#define DCC_TEXT_INPUT_PLACEHOLDER(custom_id_, label_, style_, placeholder_, required_) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_TEXT_INPUT, \
        .custom_id = (custom_id_), \
        .label = (label_), \
        .text_input_style = (style_), \
        .placeholder = (placeholder_), \
        .required = (required_), \
        .has_text_input_style = 1U, \
        .has_required = 1U \
    })
#define DCC_TEXT_INPUT_PLACEHOLDER_NS(namespace_, action_, label_, style_, placeholder_, required_) \
    DCC_TEXT_INPUT_PLACEHOLDER( \
        DCC_COMPONENT_ID(namespace_, action_), \
        (label_), \
        (style_), \
        (placeholder_), \
        (required_) \
    )

#define DCC_V2_TEXT(content_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_TEXT_DISPLAY, \
        .content = (content_) \
    })
#define DCC_V2_BUTTON(style_, label_, custom_id_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_BUTTON, \
        .button_style = (style_), \
        .custom_id = (custom_id_), \
        .label = (label_), \
        .has_button_style = 1U \
    })
#define DCC_V2_BUTTON_NS(style_, label_, namespace_, action_) \
    DCC_V2_BUTTON((style_), (label_), DCC_COMPONENT_ID(namespace_, action_))
#define DCC_V2_BUTTON_PRIMARY(label_, custom_id_) \
    DCC_V2_BUTTON(DCC_BUTTON_PRIMARY, (label_), (custom_id_))
#define DCC_V2_BUTTON_PRIMARY_NS(label_, namespace_, action_) \
    DCC_V2_BUTTON_NS(DCC_BUTTON_PRIMARY, (label_), namespace_, action_)
#define DCC_V2_BUTTON_SECONDARY(label_, custom_id_) \
    DCC_V2_BUTTON(DCC_BUTTON_SECONDARY, (label_), (custom_id_))
#define DCC_V2_BUTTON_SECONDARY_NS(label_, namespace_, action_) \
    DCC_V2_BUTTON_NS(DCC_BUTTON_SECONDARY, (label_), namespace_, action_)
#define DCC_V2_BUTTON_SUCCESS(label_, custom_id_) \
    DCC_V2_BUTTON(DCC_BUTTON_SUCCESS, (label_), (custom_id_))
#define DCC_V2_BUTTON_SUCCESS_NS(label_, namespace_, action_) \
    DCC_V2_BUTTON_NS(DCC_BUTTON_SUCCESS, (label_), namespace_, action_)
#define DCC_V2_BUTTON_DANGER(label_, custom_id_) \
    DCC_V2_BUTTON(DCC_BUTTON_DANGER, (label_), (custom_id_))
#define DCC_V2_BUTTON_DANGER_NS(label_, namespace_, action_) \
    DCC_V2_BUTTON_NS(DCC_BUTTON_DANGER, (label_), namespace_, action_)
#define DCC_V2_LINK_BUTTON(label_, url_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_BUTTON, \
        .button_style = DCC_BUTTON_LINK, \
        .label = (label_), \
        .url = (url_), \
        .has_button_style = 1U \
    })
#define DCC_V2_PREMIUM_BUTTON(sku_id_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_BUTTON, \
        .button_style = DCC_BUTTON_PREMIUM, \
        .sku_id = (sku_id_), \
        .has_button_style = 1U, \
        .has_sku_id = 1U \
    })
#define DCC_V2_ACTION_ROW_ARRAY(children_, child_count_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_ACTION_ROW, \
        .children = (children_), \
        .children_count = (child_count_) \
    })
#define DCC_V2_ACTION_ROW(...) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_ACTION_ROW, \
        .children = DCC_SUGAR_ARRAY(dcc_component_v2_builder_t, __VA_ARGS__), \
        .children_count = DCC_SUGAR_ARRAY_LEN(dcc_component_v2_builder_t, __VA_ARGS__) \
    })
#define DCC_V2_SECTION_ARRAY(children_, child_count_, accessory_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_SECTION, \
        .children = (children_), \
        .children_count = (child_count_), \
        .accessory = (accessory_) \
    })
#define DCC_V2_SECTION(accessory_, ...) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_SECTION, \
        .children = DCC_SUGAR_ARRAY(dcc_component_v2_builder_t, __VA_ARGS__), \
        .children_count = DCC_SUGAR_ARRAY_LEN(dcc_component_v2_builder_t, __VA_ARGS__), \
        .accessory = DCC_SUGAR_PTR(dcc_component_v2_builder_t, accessory_) \
    })
#define DCC_V2_CONTAINER_ARRAY(children_, child_count_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_CONTAINER, \
        .children = (children_), \
        .children_count = (child_count_) \
    })
#define DCC_V2_CONTAINER(...) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_CONTAINER, \
        .children = DCC_SUGAR_ARRAY(dcc_component_v2_builder_t, __VA_ARGS__), \
        .children_count = DCC_SUGAR_ARRAY_LEN(dcc_component_v2_builder_t, __VA_ARGS__) \
    })
#define DCC_V2_CONTAINER_ACCENT_ARRAY(children_, child_count_, accent_color_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_CONTAINER, \
        .children = (children_), \
        .children_count = (child_count_), \
        .accent_color = (accent_color_), \
        .has_accent_color = 1U \
    })
#define DCC_V2_CONTAINER_ACCENT(accent_color_, ...) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_CONTAINER, \
        .children = DCC_SUGAR_ARRAY(dcc_component_v2_builder_t, __VA_ARGS__), \
        .children_count = DCC_SUGAR_ARRAY_LEN(dcc_component_v2_builder_t, __VA_ARGS__), \
        .accent_color = (accent_color_), \
        .has_accent_color = 1U \
    })
#define DCC_V2_SEPARATOR(divider_, spacing_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_SEPARATOR, \
        .divider = (divider_), \
        .spacing = (spacing_), \
        .has_divider = 1U, \
        .has_spacing = 1U \
    })
#define DCC_V2_SEPARATOR_SMALL() \
    DCC_V2_SEPARATOR(1U, DCC_COMPONENT_V2_SPACING_SMALL)
#define DCC_V2_SEPARATOR_LARGE() \
    DCC_V2_SEPARATOR(1U, DCC_COMPONENT_V2_SPACING_LARGE)
#define DCC_V2_MEDIA(url_, description_) \
    ((dcc_component_v2_media_t){ \
        .url = (url_), \
        .description = (description_) \
    })
#define DCC_V2_MEDIA_SPOILER(url_, description_) \
    ((dcc_component_v2_media_t){ \
        .url = (url_), \
        .description = (description_), \
        .spoiler = 1U, \
        .has_spoiler = 1U \
    })
#define DCC_V2_MEDIA_GALLERY_ARRAY(media_, media_count_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_MEDIA_GALLERY, \
        .media = (media_), \
        .media_count = (media_count_) \
    })
#define DCC_V2_MEDIA_GALLERY(...) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_MEDIA_GALLERY, \
        .media = DCC_SUGAR_ARRAY(dcc_component_v2_media_t, __VA_ARGS__), \
        .media_count = DCC_SUGAR_ARRAY_LEN(dcc_component_v2_media_t, __VA_ARGS__) \
    })
#define DCC_V2_THUMBNAIL_ARRAY(media_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_THUMBNAIL, \
        .media = (media_), \
        .media_count = 1U \
    })
#define DCC_V2_THUMBNAIL(media_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_THUMBNAIL, \
        .media = DCC_SUGAR_PTR(dcc_component_v2_media_t, media_), \
        .media_count = 1U \
    })
#define DCC_V2_FILE_ARRAY(media_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_FILE, \
        .media = (media_), \
        .media_count = 1U \
    })
#define DCC_V2_FILE(media_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_FILE, \
        .media = DCC_SUGAR_PTR(dcc_component_v2_media_t, media_), \
        .media_count = 1U \
    })
#define DCC_V2_DEFAULT_USER(id_) \
    ((dcc_component_v2_select_default_value_t){ \
        .id = (id_), \
        .type = DCC_COMPONENT_V2_SELECT_DEFAULT_USER \
    })
#define DCC_V2_DEFAULT_ROLE(id_) \
    ((dcc_component_v2_select_default_value_t){ \
        .id = (id_), \
        .type = DCC_COMPONENT_V2_SELECT_DEFAULT_ROLE \
    })
#define DCC_V2_DEFAULT_CHANNEL(id_) \
    ((dcc_component_v2_select_default_value_t){ \
        .id = (id_), \
        .type = DCC_COMPONENT_V2_SELECT_DEFAULT_CHANNEL \
    })
#define DCC_V2_DEFAULT_VALUES(...) \
    DCC_SUGAR_ARRAY(dcc_component_v2_select_default_value_t, __VA_ARGS__)
#define DCC_V2_CHANNEL_TYPES(...) \
    DCC_SUGAR_ARRAY(uint32_t, __VA_ARGS__)
#define DCC_V2_STRING_SELECT_ARRAY(custom_id_, options_, option_count_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_STRING_SELECT, \
        .custom_id = (custom_id_), \
        .options = (options_), \
        .options_count = (option_count_) \
    })
#define DCC_V2_STRING_SELECT(custom_id_, ...) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_STRING_SELECT, \
        .custom_id = (custom_id_), \
        .options = DCC_SUGAR_ARRAY(dcc_select_option_t, __VA_ARGS__), \
        .options_count = DCC_SUGAR_ARRAY_LEN(dcc_select_option_t, __VA_ARGS__) \
    })
#define DCC_V2_STRING_SELECT_NS(namespace_, action_, ...) \
    DCC_V2_STRING_SELECT(DCC_COMPONENT_ID(namespace_, action_), __VA_ARGS__)
#define DCC_V2_USER_SELECT(custom_id_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_USER_SELECT, \
        .custom_id = (custom_id_) \
    })
#define DCC_V2_USER_SELECT_NS(namespace_, action_) \
    DCC_V2_USER_SELECT(DCC_COMPONENT_ID(namespace_, action_))
#define DCC_V2_ROLE_SELECT(custom_id_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_ROLE_SELECT, \
        .custom_id = (custom_id_) \
    })
#define DCC_V2_ROLE_SELECT_NS(namespace_, action_) \
    DCC_V2_ROLE_SELECT(DCC_COMPONENT_ID(namespace_, action_))
#define DCC_V2_MENTIONABLE_SELECT(custom_id_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_MENTIONABLE_SELECT, \
        .custom_id = (custom_id_) \
    })
#define DCC_V2_MENTIONABLE_SELECT_NS(namespace_, action_) \
    DCC_V2_MENTIONABLE_SELECT(DCC_COMPONENT_ID(namespace_, action_))
#define DCC_V2_CHANNEL_SELECT_ARRAY(custom_id_, default_values_, default_value_count_, channel_types_, channel_type_count_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_CHANNEL_SELECT, \
        .custom_id = (custom_id_), \
        .default_values = (default_values_), \
        .default_value_count = (default_value_count_), \
        .channel_types = (channel_types_), \
        .channel_type_count = (channel_type_count_) \
    })
#define DCC_V2_CHANNEL_SELECT(custom_id_, default_values_, channel_types_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_CHANNEL_SELECT, \
        .custom_id = (custom_id_), \
        .default_values = (default_values_), \
        .default_value_count = DCC_ARRAY_LEN(default_values_), \
        .channel_types = (channel_types_), \
        .channel_type_count = DCC_ARRAY_LEN(channel_types_) \
    })
#define DCC_V2_CHANNEL_SELECT_NS(namespace_, action_, default_values_, channel_types_) \
    DCC_V2_CHANNEL_SELECT(DCC_COMPONENT_ID(namespace_, action_), (default_values_), (channel_types_))
#define DCC_V2_TEXT_INPUT(custom_id_, label_, style_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_TEXT_INPUT, \
        .custom_id = (custom_id_), \
        .label = (label_), \
        .text_input_style = (style_), \
        .has_text_input_style = 1U \
    })
#define DCC_V2_TEXT_INPUT_NS(namespace_, action_, label_, style_) \
    DCC_V2_TEXT_INPUT(DCC_COMPONENT_ID(namespace_, action_), (label_), (style_))
#define DCC_V2_TEXT_INPUT_REQUIRED(custom_id_, label_, style_, required_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_TEXT_INPUT, \
        .custom_id = (custom_id_), \
        .label = (label_), \
        .text_input_style = (style_), \
        .required = (required_), \
        .has_text_input_style = 1U, \
        .has_required = 1U \
    })
#define DCC_V2_TEXT_INPUT_REQUIRED_NS(namespace_, action_, label_, style_, required_) \
    DCC_V2_TEXT_INPUT_REQUIRED(DCC_COMPONENT_ID(namespace_, action_), (label_), (style_), (required_))
#define DCC_V2_TEXT_INPUT_PLACEHOLDER(custom_id_, label_, style_, placeholder_, required_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_TEXT_INPUT, \
        .custom_id = (custom_id_), \
        .label = (label_), \
        .text_input_style = (style_), \
        .placeholder = (placeholder_), \
        .required = (required_), \
        .has_text_input_style = 1U, \
        .has_required = 1U \
    })
#define DCC_V2_TEXT_INPUT_PLACEHOLDER_NS(namespace_, action_, label_, style_, placeholder_, required_) \
    DCC_V2_TEXT_INPUT_PLACEHOLDER( \
        DCC_COMPONENT_ID(namespace_, action_), \
        (label_), \
        (style_), \
        (placeholder_), \
        (required_) \
    )
#define DCC_V2_LABEL_ARRAY(label_, component_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_LABEL, \
        .label = (label_), \
        .children = (component_), \
        .children_count = 1U \
    })
#define DCC_V2_LABEL(label_, component_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_LABEL, \
        .label = (label_), \
        .children = DCC_SUGAR_PTR(dcc_component_v2_builder_t, component_), \
        .children_count = 1U \
    })
#define DCC_V2_FILE_UPLOAD(custom_id_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_FILE_UPLOAD, \
        .custom_id = (custom_id_) \
    })
#define DCC_V2_FILE_UPLOAD_NS(namespace_, action_) \
    DCC_V2_FILE_UPLOAD(DCC_COMPONENT_ID(namespace_, action_))
#define DCC_V2_RADIO_GROUP_ARRAY(custom_id_, options_, option_count_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_RADIO_GROUP, \
        .custom_id = (custom_id_), \
        .options = (options_), \
        .options_count = (option_count_) \
    })
#define DCC_V2_RADIO_GROUP(custom_id_, ...) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_RADIO_GROUP, \
        .custom_id = (custom_id_), \
        .options = DCC_SUGAR_ARRAY(dcc_select_option_t, __VA_ARGS__), \
        .options_count = DCC_SUGAR_ARRAY_LEN(dcc_select_option_t, __VA_ARGS__) \
    })
#define DCC_V2_RADIO_GROUP_NS(namespace_, action_, ...) \
    DCC_V2_RADIO_GROUP(DCC_COMPONENT_ID(namespace_, action_), __VA_ARGS__)
#define DCC_V2_CHECKBOX_GROUP_ARRAY(custom_id_, options_, option_count_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_CHECKBOX_GROUP, \
        .custom_id = (custom_id_), \
        .options = (options_), \
        .options_count = (option_count_) \
    })
#define DCC_V2_CHECKBOX_GROUP(custom_id_, ...) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_CHECKBOX_GROUP, \
        .custom_id = (custom_id_), \
        .options = DCC_SUGAR_ARRAY(dcc_select_option_t, __VA_ARGS__), \
        .options_count = DCC_SUGAR_ARRAY_LEN(dcc_select_option_t, __VA_ARGS__) \
    })
#define DCC_V2_CHECKBOX_GROUP_NS(namespace_, action_, ...) \
    DCC_V2_CHECKBOX_GROUP(DCC_COMPONENT_ID(namespace_, action_), __VA_ARGS__)
#define DCC_V2_CHECKBOX(custom_id_, label_, checked_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_CHECKBOX, \
        .custom_id = (custom_id_), \
        .label = (label_), \
        .checked = (checked_), \
        .has_checked = 1U \
    })
#define DCC_V2_CHECKBOX_NS(namespace_, action_, label_, checked_) \
    DCC_V2_CHECKBOX(DCC_COMPONENT_ID(namespace_, action_), (label_), (checked_))

#define DCC_UI_TEXT(content_) DCC_V2_TEXT((content_))
#define DCC_UI_ROW(...) DCC_V2_ACTION_ROW(__VA_ARGS__)
#define DCC_UI_BUTTON(style_, label_, custom_id_) DCC_V2_BUTTON((style_), (label_), (custom_id_))
#define DCC_UI_BUTTON_NS(style_, label_, namespace_, action_) \
    DCC_V2_BUTTON_NS((style_), (label_), namespace_, action_)
#define DCC_UI_PRIMARY(label_, custom_id_) DCC_V2_BUTTON_PRIMARY((label_), (custom_id_))
#define DCC_UI_PRIMARY_NS(label_, namespace_, action_) DCC_V2_BUTTON_PRIMARY_NS((label_), namespace_, action_)
#define DCC_UI_SECONDARY(label_, custom_id_) DCC_V2_BUTTON_SECONDARY((label_), (custom_id_))
#define DCC_UI_SECONDARY_NS(label_, namespace_, action_) DCC_V2_BUTTON_SECONDARY_NS((label_), namespace_, action_)
#define DCC_UI_SUCCESS(label_, custom_id_) DCC_V2_BUTTON_SUCCESS((label_), (custom_id_))
#define DCC_UI_SUCCESS_NS(label_, namespace_, action_) DCC_V2_BUTTON_SUCCESS_NS((label_), namespace_, action_)
#define DCC_UI_DANGER(label_, custom_id_) DCC_V2_BUTTON_DANGER((label_), (custom_id_))
#define DCC_UI_DANGER_NS(label_, namespace_, action_) DCC_V2_BUTTON_DANGER_NS((label_), namespace_, action_)
#define DCC_UI_LINK(label_, url_) DCC_V2_LINK_BUTTON((label_), (url_))
#define DCC_UI_PREMIUM(sku_id_) DCC_V2_PREMIUM_BUTTON((sku_id_))
#define DCC_UI_SECTION(accessory_, ...) DCC_V2_SECTION((accessory_), __VA_ARGS__)
#define DCC_UI_CARD(...) DCC_V2_CONTAINER(__VA_ARGS__)
#define DCC_UI_CARD_ACCENT(accent_color_, ...) DCC_V2_CONTAINER_ACCENT((accent_color_), __VA_ARGS__)
#define DCC_UI_SEPARATOR() DCC_V2_SEPARATOR_SMALL()
#define DCC_UI_SEPARATOR_LARGE() DCC_V2_SEPARATOR_LARGE()
#define DCC_UI_MEDIA(url_, description_) DCC_V2_MEDIA((url_), (description_))
#define DCC_UI_GALLERY(...) DCC_V2_MEDIA_GALLERY(__VA_ARGS__)
#define DCC_UI_STRING_SELECT(custom_id_, ...) DCC_V2_STRING_SELECT((custom_id_), __VA_ARGS__)
#define DCC_UI_STRING_SELECT_NS(namespace_, action_, ...) \
    DCC_V2_STRING_SELECT_NS(namespace_, action_, __VA_ARGS__)
#define DCC_UI_USER_SELECT(custom_id_) DCC_V2_USER_SELECT((custom_id_))
#define DCC_UI_USER_SELECT_NS(namespace_, action_) DCC_V2_USER_SELECT_NS(namespace_, action_)
#define DCC_UI_ROLE_SELECT(custom_id_) DCC_V2_ROLE_SELECT((custom_id_))
#define DCC_UI_ROLE_SELECT_NS(namespace_, action_) DCC_V2_ROLE_SELECT_NS(namespace_, action_)
#define DCC_UI_MENTIONABLE_SELECT(custom_id_) DCC_V2_MENTIONABLE_SELECT((custom_id_))
#define DCC_UI_MENTIONABLE_SELECT_NS(namespace_, action_) DCC_V2_MENTIONABLE_SELECT_NS(namespace_, action_)
#define DCC_UI_CHANNEL_SELECT(custom_id_, default_values_, channel_types_) \
    DCC_V2_CHANNEL_SELECT((custom_id_), (default_values_), (channel_types_))
#define DCC_UI_CHANNEL_SELECT_NS(namespace_, action_, default_values_, channel_types_) \
    DCC_V2_CHANNEL_SELECT_NS(namespace_, action_, (default_values_), (channel_types_))
#define DCC_UI_LABEL(label_, component_) DCC_V2_LABEL((label_), (component_))
#define DCC_UI_INPUT(custom_id_, label_, required_) \
    DCC_MODAL_V2_TEXT_INPUT((custom_id_), (label_), (required_))
#define DCC_UI_INPUT_NS(namespace_, action_, label_, required_) \
    DCC_MODAL_V2_TEXT_INPUT(DCC_COMPONENT_ID(namespace_, action_), (label_), (required_))
#define DCC_UI_INPUT_PLACEHOLDER(custom_id_, label_, placeholder_, required_) \
    DCC_MODAL_V2_TEXT_INPUT_PLACEHOLDER((custom_id_), (label_), (placeholder_), (required_))
#define DCC_UI_INPUT_PLACEHOLDER_NS(namespace_, action_, label_, placeholder_, required_) \
    DCC_MODAL_V2_TEXT_INPUT_PLACEHOLDER( \
        DCC_COMPONENT_ID(namespace_, action_), \
        (label_), \
        (placeholder_), \
        (required_) \
    )
#define DCC_UI_CHECKBOX(custom_id_, label_, checked_) DCC_MODAL_V2_CHECKBOX((custom_id_), (label_), (checked_))
#define DCC_UI_CHECKBOX_NS(namespace_, action_, label_, checked_) \
    DCC_MODAL_V2_CHECKBOX(DCC_COMPONENT_ID(namespace_, action_), (label_), (checked_))
#define DCC_UI_MESSAGE(...) DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__)
#define DCC_UI_EPHEMERAL(...) DCC_MESSAGE_EPHEMERAL_COMPONENTS_V2(__VA_ARGS__)
#define DCC_RESPOND_UI(ctx_, ...) DCC_RESPOND_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_UI_CB(ctx_, cb_, user_data_, ...) \
    DCC_RESPOND_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_RESPOND_OR_EDIT_UI(ctx_, ...) DCC_RESPOND_OR_EDIT_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_EDIT_UI_CB(ctx_, cb_, user_data_, ...) \
    DCC_RESPOND_OR_EDIT_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_RESPOND_OR_FOLLOWUP_UI(ctx_, ...) DCC_RESPOND_OR_FOLLOWUP_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_FOLLOWUP_UI_CB(ctx_, cb_, user_data_, ...) \
    DCC_RESPOND_OR_FOLLOWUP_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_RESPOND_EPHEMERAL_UI(ctx_, ...) DCC_RESPOND_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_PRIVATE_UI(ctx_, ...) DCC_RESPOND_PRIVATE_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_EDIT_EPHEMERAL_UI(ctx_, ...) \
    DCC_RESPOND_OR_EDIT_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_EDIT_PRIVATE_UI(ctx_, ...) \
    DCC_RESPOND_OR_EDIT_PRIVATE_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_FOLLOWUP_EPHEMERAL_UI(ctx_, ...) \
    DCC_RESPOND_OR_FOLLOWUP_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_FOLLOWUP_PRIVATE_UI(ctx_, ...) \
    DCC_RESPOND_OR_FOLLOWUP_PRIVATE_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_REPLY_UI(ctx_, ...) DCC_CTX_REPLY_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_UI(ctx_, ...) DCC_CTX_RESPOND_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_UI_CB(ctx_, cb_, user_data_, ...) \
    DCC_CTX_RESPOND_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_EDIT_UI(ctx_, ...) DCC_CTX_RESPOND_OR_EDIT_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_EDIT_UI_CB(ctx_, cb_, user_data_, ...) \
    DCC_CTX_RESPOND_OR_EDIT_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_FOLLOWUP_UI(ctx_, ...) \
    DCC_CTX_RESPOND_OR_FOLLOWUP_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_FOLLOWUP_UI_CB(ctx_, cb_, user_data_, ...) \
    DCC_CTX_RESPOND_OR_FOLLOWUP_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_EPHEMERAL_UI(ctx_, ...) DCC_CTX_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_EPHEMERAL_UI(ctx_, ...) \
    DCC_CTX_RESPOND_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_PRIVATE_UI(ctx_, ...) DCC_CTX_RESPOND_PRIVATE_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_EDIT_EPHEMERAL_UI(ctx_, ...) \
    DCC_CTX_RESPOND_OR_EDIT_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_EDIT_PRIVATE_UI(ctx_, ...) \
    DCC_CTX_RESPOND_OR_EDIT_PRIVATE_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_FOLLOWUP_EPHEMERAL_UI(ctx_, ...) \
    DCC_CTX_RESPOND_OR_FOLLOWUP_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_FOLLOWUP_PRIVATE_UI(ctx_, ...) \
    DCC_CTX_RESPOND_OR_FOLLOWUP_PRIVATE_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_UPDATE_UI(ctx_, ...) DCC_CTX_UPDATE_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_FOLLOWUP_UI(ctx_, ...) DCC_CTX_FOLLOWUP_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_SEND_UI(ctx_, ...) DCC_CTX_SEND_V2((ctx_), __VA_ARGS__)
#define DCC_SEND_UI_WITH_ID(ctx_, cb_, user_data_, ...) \
    DCC_SEND_V2_WITH_ID((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_SEND_UI_ID(ctx_, cb_, user_data_, ...) \
    DCC_SEND_UI_WITH_ID((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_SEND_UI_WITH_ID(ctx_, cb_, user_data_, ...) \
    DCC_CTX_SEND_V2_WITH_ID((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_SEND_UI_ID(ctx_, cb_, user_data_, ...) \
    DCC_CTX_SEND_UI_WITH_ID((ctx_), (cb_), (user_data_), __VA_ARGS__)

#define DCC_MODAL_BUILDER_ARRAY(custom_id_, title_, components_, component_count_) \
    ((dcc_modal_builder_t){ \
        .custom_id = (custom_id_), \
        .title = (title_), \
        .components = (components_), \
        .components_count = (component_count_), \
        .has_custom_id = 1U, \
        .has_title = 1U \
    })
#define DCC_MODAL_BUILDER(custom_id_, title_, ...) \
    ((dcc_modal_builder_t){ \
        .custom_id = (custom_id_), \
        .title = (title_), \
        .components = DCC_SUGAR_ARRAY(dcc_component_builder_t, __VA_ARGS__), \
        .components_count = DCC_SUGAR_ARRAY_LEN(dcc_component_builder_t, __VA_ARGS__), \
        .has_custom_id = 1U, \
        .has_title = 1U \
    })
#define DCC_MODAL_V2_BUILDER_ARRAY(custom_id_, title_, components_, component_count_) \
    ((dcc_modal_builder_t){ \
        .custom_id = (custom_id_), \
        .title = (title_), \
        .components_v2 = (components_), \
        .components_v2_count = (component_count_), \
        .has_custom_id = 1U, \
        .has_title = 1U \
    })
#define DCC_MODAL_V2_BUILDER(custom_id_, title_, ...) \
    ((dcc_modal_builder_t){ \
        .custom_id = (custom_id_), \
        .title = (title_), \
        .components_v2 = DCC_SUGAR_ARRAY(dcc_component_v2_builder_t, __VA_ARGS__), \
        .components_v2_count = DCC_SUGAR_ARRAY_LEN(dcc_component_v2_builder_t, __VA_ARGS__), \
        .has_custom_id = 1U, \
        .has_title = 1U \
    })
