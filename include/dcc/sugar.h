#ifndef DCC_SUGAR_H
#define DCC_SUGAR_H

#include <dcc/application_command.h>
#include <dcc/client.h>
#include <dcc/command_registry.h>
#include <dcc/component.h>
#include <dcc/component_session.h>
#include <dcc/component_v2.h>
#include <dcc/embed.h>
#include <dcc/hot_reload.h>
#include <dcc/intents.h>
#include <dcc/interaction_flow.h>
#include <dcc/message.h>
#include <dcc/modal.h>
#include <dcc/replay.h>
#include <dcc/rest/application_commands/registration.h>
#include <dcc/rest/firewall.h>
#include <stdint.h>

#define DCC_ARRAY_LEN(array_) (sizeof(array_) / sizeof((array_)[0]))

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

#define DCC_CLIENT_SHARDED_OPTIONS(token_, intents_, shard_id_, shard_count_) \
    ((dcc_client_options_t){ \
        .size = sizeof(dcc_client_options_t), \
        .token = (token_), \
        .intents = (intents_), \
        .shard_id = (shard_id_), \
        .shard_count = (shard_count_) \
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
#define DCC_EMBED_WITH_FIELDS(title_, description_, fields_, field_count_) \
    ((dcc_embed_builder_t){ \
        .title = (title_), \
        .description = (description_), \
        .fields = (fields_), \
        .field_count = (field_count_) \
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
#define DCC_MESSAGE_FLAGS(flags_) \
    ((dcc_message_builder_t){ \
        .flags = (flags_), \
        .has_flags = 1U \
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
#define DCC_MESSAGE_EMBEDS(embeds_, embed_count_) \
    ((dcc_message_builder_t){ \
        .embeds = (embeds_), \
        .embeds_count = (embed_count_) \
    })
#define DCC_MESSAGE_TEXT_EMBEDS(content_, embeds_, embed_count_) \
    ((dcc_message_builder_t){ \
        .content = (content_), \
        .embeds = (embeds_), \
        .embeds_count = (embed_count_), \
        .has_content = 1U \
    })
#define DCC_MESSAGE_COMPONENTS(components_, component_count_) \
    ((dcc_message_builder_t){ \
        .components = (components_), \
        .components_count = (component_count_) \
    })
#define DCC_MESSAGE_COMPONENTS_JSON(components_json_) \
    ((dcc_message_builder_t){ \
        .components_json = (components_json_) \
    })
#define DCC_MESSAGE_COMPONENTS_V2(components_, component_count_) \
    ((dcc_message_builder_t){ \
        .components_v2 = (components_), \
        .components_v2_count = (component_count_), \
        .flags = DCC_MESSAGE_FLAG_IS_COMPONENTS_V2, \
        .has_flags = 1U \
    })
#define DCC_MESSAGE_COMPONENTS_V2_JSON(components_json_) \
    ((dcc_message_builder_t){ \
        .components_v2_json = (components_json_), \
        .flags = DCC_MESSAGE_FLAG_IS_COMPONENTS_V2, \
        .has_flags = 1U \
    })
#define DCC_MESSAGE_ALLOWED_MENTIONS_JSON(content_, allowed_mentions_json_) \
    ((dcc_message_builder_t){ \
        .content = (content_), \
        .allowed_mentions_json = (allowed_mentions_json_), \
        .has_content = 1U \
    })
#define DCC_MESSAGE_STICKERS(sticker_ids_, sticker_count_) \
    ((dcc_message_builder_t){ \
        .sticker_ids = (sticker_ids_), \
        .sticker_ids_count = (sticker_count_) \
    })
#define DCC_POLL_MEDIA(text_) \
    ((dcc_poll_media_t){ \
        .text = (text_) \
    })
#define DCC_POLL_MEDIA_EMOJI(text_, emoji_) \
    ((dcc_poll_media_t){ \
        .text = (text_), \
        .emoji = (emoji_), \
        .has_emoji = 1U \
    })
#define DCC_POLL_ANSWER(text_) \
    ((dcc_poll_answer_t){ \
        .media = DCC_POLL_MEDIA(text_) \
    })
#define DCC_POLL(question_, answers_, answer_count_, duration_hours_) \
    ((dcc_poll_builder_t){ \
        .size = sizeof(dcc_poll_builder_t), \
        .question = DCC_POLL_MEDIA(question_), \
        .answers = (answers_), \
        .answer_count = (answer_count_), \
        .duration_hours = (duration_hours_), \
        .layout_type = DCC_POLL_LAYOUT_DEFAULT, \
        .has_duration = 1U, \
        .has_layout_type = 1U \
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

#define DCC_ACTION_ROW(children_, child_count_) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_ACTION_ROW, \
        .children = (children_), \
        .children_count = (child_count_) \
    })
#define DCC_BUTTON_BUILDER(style_, label_, custom_id_) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_BUTTON, \
        .style = (style_), \
        .custom_id = (custom_id_), \
        .label = (label_), \
        .has_style = 1U \
    })
#define DCC_BUTTON_PRIMARY_BUILDER(label_, custom_id_) \
    DCC_BUTTON_BUILDER(DCC_BUTTON_PRIMARY, (label_), (custom_id_))
#define DCC_BUTTON_SECONDARY_BUILDER(label_, custom_id_) \
    DCC_BUTTON_BUILDER(DCC_BUTTON_SECONDARY, (label_), (custom_id_))
#define DCC_BUTTON_SUCCESS_BUILDER(label_, custom_id_) \
    DCC_BUTTON_BUILDER(DCC_BUTTON_SUCCESS, (label_), (custom_id_))
#define DCC_BUTTON_DANGER_BUILDER(label_, custom_id_) \
    DCC_BUTTON_BUILDER(DCC_BUTTON_DANGER, (label_), (custom_id_))
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
#define DCC_STRING_SELECT_BUILDER(custom_id_, options_, option_count_) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_STRING_SELECT, \
        .custom_id = (custom_id_), \
        .options = (options_), \
        .options_count = (option_count_) \
    })
#define DCC_USER_SELECT_BUILDER(custom_id_) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_USER_SELECT, \
        .custom_id = (custom_id_) \
    })
#define DCC_ROLE_SELECT_BUILDER(custom_id_) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_ROLE_SELECT, \
        .custom_id = (custom_id_) \
    })
#define DCC_MENTIONABLE_SELECT_BUILDER(custom_id_) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_MENTIONABLE_SELECT, \
        .custom_id = (custom_id_) \
    })
#define DCC_CHANNEL_SELECT_BUILDER(custom_id_) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_CHANNEL_SELECT, \
        .custom_id = (custom_id_) \
    })
#define DCC_TEXT_INPUT_BUILDER(custom_id_, label_, style_) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_TEXT_INPUT, \
        .custom_id = (custom_id_), \
        .label = (label_), \
        .text_input_style = (style_), \
        .has_text_input_style = 1U \
    })

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
#define DCC_V2_BUTTON_PRIMARY(label_, custom_id_) \
    DCC_V2_BUTTON(DCC_BUTTON_PRIMARY, (label_), (custom_id_))
#define DCC_V2_BUTTON_SECONDARY(label_, custom_id_) \
    DCC_V2_BUTTON(DCC_BUTTON_SECONDARY, (label_), (custom_id_))
#define DCC_V2_BUTTON_SUCCESS(label_, custom_id_) \
    DCC_V2_BUTTON(DCC_BUTTON_SUCCESS, (label_), (custom_id_))
#define DCC_V2_BUTTON_DANGER(label_, custom_id_) \
    DCC_V2_BUTTON(DCC_BUTTON_DANGER, (label_), (custom_id_))
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
#define DCC_V2_ACTION_ROW(children_, child_count_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_ACTION_ROW, \
        .children = (children_), \
        .children_count = (child_count_) \
    })
#define DCC_V2_SECTION(children_, child_count_, accessory_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_SECTION, \
        .children = (children_), \
        .children_count = (child_count_), \
        .accessory = (accessory_) \
    })
#define DCC_V2_CONTAINER(children_, child_count_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_CONTAINER, \
        .children = (children_), \
        .children_count = (child_count_) \
    })
#define DCC_V2_CONTAINER_ACCENT(children_, child_count_, accent_color_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_CONTAINER, \
        .children = (children_), \
        .children_count = (child_count_), \
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
#define DCC_V2_MEDIA_GALLERY(media_, media_count_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_MEDIA_GALLERY, \
        .media = (media_), \
        .media_count = (media_count_) \
    })
#define DCC_V2_THUMBNAIL(media_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_THUMBNAIL, \
        .media = (media_), \
        .media_count = 1U \
    })
#define DCC_V2_FILE(media_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_FILE, \
        .media = (media_), \
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
#define DCC_V2_STRING_SELECT(custom_id_, options_, option_count_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_STRING_SELECT, \
        .custom_id = (custom_id_), \
        .options = (options_), \
        .options_count = (option_count_) \
    })
#define DCC_V2_USER_SELECT(custom_id_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_USER_SELECT, \
        .custom_id = (custom_id_) \
    })
#define DCC_V2_ROLE_SELECT(custom_id_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_ROLE_SELECT, \
        .custom_id = (custom_id_) \
    })
#define DCC_V2_MENTIONABLE_SELECT(custom_id_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_MENTIONABLE_SELECT, \
        .custom_id = (custom_id_) \
    })
#define DCC_V2_CHANNEL_SELECT(custom_id_, default_values_, default_value_count_, channel_types_, channel_type_count_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_CHANNEL_SELECT, \
        .custom_id = (custom_id_), \
        .default_values = (default_values_), \
        .default_value_count = (default_value_count_), \
        .channel_types = (channel_types_), \
        .channel_type_count = (channel_type_count_) \
    })
#define DCC_V2_TEXT_INPUT(custom_id_, label_, style_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_TEXT_INPUT, \
        .custom_id = (custom_id_), \
        .label = (label_), \
        .text_input_style = (style_), \
        .has_text_input_style = 1U \
    })
#define DCC_V2_LABEL(label_, component_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_LABEL, \
        .label = (label_), \
        .children = (component_), \
        .children_count = 1U \
    })
#define DCC_V2_FILE_UPLOAD(custom_id_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_FILE_UPLOAD, \
        .custom_id = (custom_id_) \
    })
#define DCC_V2_RADIO_GROUP(custom_id_, options_, option_count_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_RADIO_GROUP, \
        .custom_id = (custom_id_), \
        .options = (options_), \
        .options_count = (option_count_) \
    })
#define DCC_V2_CHECKBOX_GROUP(custom_id_, options_, option_count_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_CHECKBOX_GROUP, \
        .custom_id = (custom_id_), \
        .options = (options_), \
        .options_count = (option_count_) \
    })
#define DCC_V2_CHECKBOX(custom_id_, label_, checked_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_CHECKBOX, \
        .custom_id = (custom_id_), \
        .label = (label_), \
        .checked = (checked_), \
        .has_checked = 1U \
    })

#define DCC_MODAL_BUILDER(custom_id_, title_, components_, component_count_) \
    ((dcc_modal_builder_t){ \
        .custom_id = (custom_id_), \
        .title = (title_), \
        .components = (components_), \
        .components_count = (component_count_), \
        .has_custom_id = 1U, \
        .has_title = 1U \
    })
#define DCC_MODAL_V2_BUILDER(custom_id_, title_, components_, component_count_) \
    ((dcc_modal_builder_t){ \
        .custom_id = (custom_id_), \
        .title = (title_), \
        .components_v2 = (components_), \
        .components_v2_count = (component_count_), \
        .has_custom_id = 1U, \
        .has_title = 1U \
    })
#define DCC_MODAL_COMPONENTS_JSON(custom_id_, title_, components_json_) \
    ((dcc_modal_builder_t){ \
        .custom_id = (custom_id_), \
        .title = (title_), \
        .components_json = (components_json_), \
        .has_custom_id = 1U, \
        .has_title = 1U \
    })
#define DCC_MODAL_COMPONENTS_V2_JSON(custom_id_, title_, components_json_) \
    ((dcc_modal_builder_t){ \
        .custom_id = (custom_id_), \
        .title = (title_), \
        .components_v2_json = (components_json_), \
        .has_custom_id = 1U, \
        .has_title = 1U \
    })

#define DCC_SLASH_COMMAND(name_, description_) \
    ((dcc_application_command_builder_t){ \
        .name = (name_), \
        .description = (description_), \
        .type = DCC_APPLICATION_COMMAND_CHAT_INPUT, \
        .has_name = 1U, \
        .has_description = 1U, \
        .has_type = 1U \
    })
#define DCC_SLASH_COMMAND_OPTIONS(name_, description_, options_json_) \
    ((dcc_application_command_builder_t){ \
        .name = (name_), \
        .description = (description_), \
        .options_json = (options_json_), \
        .type = DCC_APPLICATION_COMMAND_CHAT_INPUT, \
        .has_name = 1U, \
        .has_description = 1U, \
        .has_type = 1U \
    })
#define DCC_SLASH_COMMAND_NSFW(name_, description_) \
    ((dcc_application_command_builder_t){ \
        .name = (name_), \
        .description = (description_), \
        .type = DCC_APPLICATION_COMMAND_CHAT_INPUT, \
        .nsfw = 1U, \
        .has_name = 1U, \
        .has_description = 1U, \
        .has_type = 1U, \
        .has_nsfw = 1U \
    })
#define DCC_SLASH_COMMAND_DM(name_, description_, dm_permission_) \
    ((dcc_application_command_builder_t){ \
        .name = (name_), \
        .description = (description_), \
        .type = DCC_APPLICATION_COMMAND_CHAT_INPUT, \
        .dm_permission = (dm_permission_), \
        .has_name = 1U, \
        .has_description = 1U, \
        .has_type = 1U, \
        .has_dm_permission = 1U \
    })
#define DCC_SLASH_COMMAND_DEFAULT_PERMISSIONS(name_, description_, permissions_) \
    ((dcc_application_command_builder_t){ \
        .name = (name_), \
        .description = (description_), \
        .type = DCC_APPLICATION_COMMAND_CHAT_INPUT, \
        .default_member_permissions = (permissions_), \
        .has_name = 1U, \
        .has_description = 1U, \
        .has_type = 1U, \
        .has_default_member_permissions = 1U \
    })
#define DCC_USER_COMMAND_BUILDER(name_) \
    ((dcc_application_command_builder_t){ \
        .name = (name_), \
        .type = DCC_APPLICATION_COMMAND_USER, \
        .has_name = 1U, \
        .has_type = 1U \
    })
#define DCC_MESSAGE_COMMAND_BUILDER(name_) \
    ((dcc_application_command_builder_t){ \
        .name = (name_), \
        .type = DCC_APPLICATION_COMMAND_MESSAGE, \
        .has_name = 1U, \
        .has_type = 1U \
    })

#define DCC_COMMAND_REGISTRY_OPTIONS_GLOBAL() \
    ((dcc_command_registry_options_t){ \
        .size = sizeof(dcc_command_registry_options_t) \
    })
#define DCC_COMMAND_REGISTRY_OPTIONS_GUILD(guild_id_) \
    ((dcc_command_registry_options_t){ \
        .size = sizeof(dcc_command_registry_options_t), \
        .guild_id = (guild_id_) \
    })
#define DCC_COMMAND_REGISTRY_OPTIONS_GLOBAL_DRY_RUN() \
    ((dcc_command_registry_options_t){ \
        .size = sizeof(dcc_command_registry_options_t), \
        .dry_run = 1U \
    })
#define DCC_COMMAND_REGISTRY_OPTIONS_GUILD_DRY_RUN(guild_id_) \
    ((dcc_command_registry_options_t){ \
        .size = sizeof(dcc_command_registry_options_t), \
        .guild_id = (guild_id_), \
        .dry_run = 1U \
    })
#define DCC_COMMAND_REGISTRY_OPTIONS_GLOBAL_DELETE_STALE() \
    ((dcc_command_registry_options_t){ \
        .size = sizeof(dcc_command_registry_options_t), \
        .delete_stale = 1U \
    })
#define DCC_COMMAND_REGISTRY_OPTIONS_GUILD_DELETE_STALE(guild_id_) \
    ((dcc_command_registry_options_t){ \
        .size = sizeof(dcc_command_registry_options_t), \
        .guild_id = (guild_id_), \
        .delete_stale = 1U \
    })
#define DCC_APPLICATION_COMMAND_REGISTRATION_GLOBAL() \
    ((dcc_application_command_registration_options_t){ \
        .size = sizeof(dcc_application_command_registration_options_t) \
    })
#define DCC_APPLICATION_COMMAND_REGISTRATION_GUILD(guild_id_) \
    ((dcc_application_command_registration_options_t){ \
        .size = sizeof(dcc_application_command_registration_options_t), \
        .guild_id = (guild_id_) \
    })

#define DCC_COMPONENT_SESSION_OPTIONS(secret_, secret_len_) \
    ((dcc_component_session_options_t){ \
        .size = sizeof(dcc_component_session_options_t), \
        .secret = (secret_), \
        .secret_len = (secret_len_), \
        .ttl_ms = UINT64_C(900000) \
    })
#define DCC_COMPONENT_SESSION_OPTIONS_TTL(secret_, secret_len_, now_ms_, ttl_ms_) \
    ((dcc_component_session_options_t){ \
        .size = sizeof(dcc_component_session_options_t), \
        .secret = (secret_), \
        .secret_len = (secret_len_), \
        .now_ms = (now_ms_), \
        .ttl_ms = (ttl_ms_) \
    })
#define DCC_COMPONENT_SESSION_OPTIONS_USER(secret_, secret_len_, user_id_) \
    ((dcc_component_session_options_t){ \
        .size = sizeof(dcc_component_session_options_t), \
        .secret = (secret_), \
        .secret_len = (secret_len_), \
        .ttl_ms = UINT64_C(900000), \
        .user_id = (user_id_), \
        .lock_user = 1U \
    })
#define DCC_COMPONENT_SESSION_CHECK(custom_id_, now_ms_, user_id_, channel_id_, guild_id_) \
    ((dcc_component_session_check_t){ \
        .size = sizeof(dcc_component_session_check_t), \
        .custom_id = (custom_id_), \
        .now_ms = (now_ms_), \
        .user_id = (user_id_), \
        .channel_id = (channel_id_), \
        .guild_id = (guild_id_) \
    })
#define DCC_COMPONENT_SESSION_LISTENER_OPTIONS() \
    ((dcc_component_session_listener_options_t){ \
        .size = sizeof(dcc_component_session_listener_options_t), \
        .listen_buttons = 1U, \
        .listen_selects = 1U, \
        .listen_forms = 1U \
    })

#define DCC_FLOW(client_, interaction_) \
    ((dcc_interaction_flow_t){ \
        .size = sizeof(dcc_interaction_flow_t), \
        .client = (client_), \
        .interaction = (interaction_), \
        .state = DCC_INTERACTION_FLOW_READY \
    })
#define DCC_FLOW_AUTO_DEFER(client_, interaction_, after_ms_) \
    ((dcc_interaction_flow_t){ \
        .size = sizeof(dcc_interaction_flow_t), \
        .client = (client_), \
        .interaction = (interaction_), \
        .state = DCC_INTERACTION_FLOW_READY, \
        .auto_defer_after_ms = (after_ms_) \
    })
#define DCC_FLOW_AUTO_DEFER_EPHEMERAL(client_, interaction_, after_ms_) \
    ((dcc_interaction_flow_t){ \
        .size = sizeof(dcc_interaction_flow_t), \
        .client = (client_), \
        .interaction = (interaction_), \
        .state = DCC_INTERACTION_FLOW_READY, \
        .auto_defer_after_ms = (after_ms_), \
        .auto_defer_ephemeral = 1U \
    })

#define DCC_HOT_RELOAD_OPTIONS() \
    ((dcc_hot_reload_options_t){ \
        .size = sizeof(dcc_hot_reload_options_t) \
    })
#define DCC_HOT_RELOAD_ISOLATED_OPTIONS(worker_path_) \
    ((dcc_hot_reload_options_t){ \
        .size = sizeof(dcc_hot_reload_options_t), \
        .backend = DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER, \
        .worker_path = (worker_path_) \
    })
#define DCC_HOT_RELOAD_ISOLATED_CANARY_OPTIONS(worker_path_, canary_options_) \
    ((dcc_hot_reload_options_t){ \
        .size = sizeof(dcc_hot_reload_options_t), \
        .backend = DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER, \
        .worker_path = (worker_path_), \
        .worker_canary_enabled = 1U, \
        .worker_canary_options = (canary_options_) \
    })
#define DCC_HOT_RELOAD_CANARY_OPTIONS(percent_, promote_after_ms_, max_error_rate_, max_consecutive_failures_) \
    ((dcc_hot_reload_canary_options_t){ \
        .size = sizeof(dcc_hot_reload_canary_options_t), \
        .canary_percent = (percent_), \
        .promote_after_ms = (promote_after_ms_), \
        .max_error_rate = (max_error_rate_), \
        .max_consecutive_failures = (max_consecutive_failures_) \
    })
#define DCC_HOT_RELOAD_CANARY_OPTIONS_DEFAULT() \
    DCC_HOT_RELOAD_CANARY_OPTIONS(5U, 30000U, 0.05, 3U)
#define DCC_HOT_RELOAD_RUN_OPTIONS(client_options_, library_path_, hot_reload_options_) \
    ((dcc_hot_reload_run_options_t){ \
        .size = sizeof(dcc_hot_reload_run_options_t), \
        .client_options = (client_options_), \
        .library_path = (library_path_), \
        .hot_reload_options = (hot_reload_options_) \
    })

#define DCC_REST_FIREWALL_OPTIONS() \
    ((dcc_rest_firewall_options_t){ \
        .size = sizeof(dcc_rest_firewall_options_t), \
        .invalid_request_soft_limit = DCC_REST_FIREWALL_DEFAULT_INVALID_REQUEST_SOFT_LIMIT, \
        .invalid_request_hard_limit = DCC_REST_FIREWALL_DEFAULT_INVALID_REQUEST_HARD_LIMIT, \
        .invalid_request_window_ms = DCC_REST_FIREWALL_DEFAULT_INVALID_REQUEST_WINDOW_MS, \
        .soft_limit_delay_ms = DCC_REST_FIREWALL_DEFAULT_SOFT_LIMIT_DELAY_MS, \
        .on_hard_limit = DCC_REST_FIREWALL_HARD_LIMIT_REJECT_NONCRITICAL \
    })
#define DCC_REST_FIREWALL_OPTIONS_STRICT() \
    ((dcc_rest_firewall_options_t){ \
        .size = sizeof(dcc_rest_firewall_options_t), \
        .invalid_request_soft_limit = DCC_REST_FIREWALL_DEFAULT_INVALID_REQUEST_SOFT_LIMIT, \
        .invalid_request_hard_limit = DCC_REST_FIREWALL_DEFAULT_INVALID_REQUEST_HARD_LIMIT, \
        .invalid_request_window_ms = DCC_REST_FIREWALL_DEFAULT_INVALID_REQUEST_WINDOW_MS, \
        .soft_limit_delay_ms = DCC_REST_FIREWALL_DEFAULT_SOFT_LIMIT_DELAY_MS, \
        .on_hard_limit = DCC_REST_FIREWALL_HARD_LIMIT_REJECT_ALL \
    })
#define DCC_REST_FIREWALL_REQUEST(method_, path_, now_ms_, critical_) \
    ((dcc_rest_firewall_request_t){ \
        .size = sizeof(dcc_rest_firewall_request_t), \
        .method = (method_), \
        .path = (path_), \
        .now_ms = (now_ms_), \
        .critical = (critical_) \
    })

#define DCC_REPLAY_GATEWAY_RECORD(event_, ts_ms_, payload_, payload_len_) \
    ((dcc_replay_record_t){ \
        .size = sizeof(dcc_replay_record_t), \
        .kind = DCC_REPLAY_GATEWAY, \
        .ts_ms = (ts_ms_), \
        .event = (event_), \
        .payload = (payload_), \
        .payload_len = (payload_len_) \
    })
#define DCC_REPLAY_INTERACTION_RECORD(ts_ms_, payload_, payload_len_) \
    ((dcc_replay_record_t){ \
        .size = sizeof(dcc_replay_record_t), \
        .kind = DCC_REPLAY_INTERACTION, \
        .ts_ms = (ts_ms_), \
        .payload = (payload_), \
        .payload_len = (payload_len_) \
    })

#endif
