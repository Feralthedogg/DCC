#ifndef DCC_SUGAR_MESSAGE_ACTIONS_H
#define DCC_SUGAR_MESSAGE_ACTIONS_H

#include <dcc/app.h>
#include <dcc/component.h>
#include <dcc/embed.h>
#include <dcc/message.h>

#define DCC_MESSAGE_COMPONENT_ACTIONS_ARRAY(content_, components_, component_count_, flags_) \
    ((dcc_message_builder_t){ \
        .content = (content_), \
        .components = (components_), \
        .components_count = (component_count_), \
        .flags = (flags_), \
        .has_content = 1U, \
        .has_flags = ((flags_) != 0U ? 1U : 0U) \
    })

#define DCC_MESSAGE_TEXT_ACTIONS(content_, ...) \
    DCC_MESSAGE_COMPONENT_ACTIONS_ARRAY( \
        (content_), \
        DCC_SUGAR_ARRAY(dcc_component_builder_t, __VA_ARGS__), \
        DCC_SUGAR_ARRAY_LEN(dcc_component_builder_t, __VA_ARGS__), \
        0U \
    )

#define DCC_MESSAGE_EPHEMERAL_TEXT_ACTIONS(content_, ...) \
    DCC_MESSAGE_COMPONENT_ACTIONS_ARRAY( \
        (content_), \
        DCC_SUGAR_ARRAY(dcc_component_builder_t, __VA_ARGS__), \
        DCC_SUGAR_ARRAY_LEN(dcc_component_builder_t, __VA_ARGS__), \
        DCC_MESSAGE_FLAG_EPHEMERAL \
    )

#define DCC_MESSAGE_EMBEDS_ACTIONS_ARRAY(embeds_, embed_count_, components_, component_count_, flags_) \
    ((dcc_message_builder_t){ \
        .embeds = (embeds_), \
        .embeds_count = (embed_count_), \
        .components = (components_), \
        .components_count = (component_count_), \
        .flags = (flags_), \
        .has_flags = ((flags_) != 0U ? 1U : 0U) \
    })

#define DCC_MESSAGE_TEXT_EMBEDS_ACTIONS_ARRAY( \
    content_, \
    embeds_, \
    embed_count_, \
    components_, \
    component_count_, \
    flags_ \
) \
    ((dcc_message_builder_t){ \
        .content = (content_), \
        .embeds = (embeds_), \
        .embeds_count = (embed_count_), \
        .components = (components_), \
        .components_count = (component_count_), \
        .flags = (flags_), \
        .has_content = 1U, \
        .has_flags = ((flags_) != 0U ? 1U : 0U) \
    })

#define DCC_MESSAGE_EMBED_ACTIONS(embed_, ...) \
    DCC_MESSAGE_EMBEDS_ACTIONS_ARRAY( \
        DCC_SUGAR_ARRAY(dcc_embed_builder_t, (embed_)), \
        1U, \
        DCC_SUGAR_ARRAY(dcc_component_builder_t, __VA_ARGS__), \
        DCC_SUGAR_ARRAY_LEN(dcc_component_builder_t, __VA_ARGS__), \
        0U \
    )

#define DCC_MESSAGE_EPHEMERAL_EMBED_ACTIONS(embed_, ...) \
    DCC_MESSAGE_EMBEDS_ACTIONS_ARRAY( \
        DCC_SUGAR_ARRAY(dcc_embed_builder_t, (embed_)), \
        1U, \
        DCC_SUGAR_ARRAY(dcc_component_builder_t, __VA_ARGS__), \
        DCC_SUGAR_ARRAY_LEN(dcc_component_builder_t, __VA_ARGS__), \
        DCC_MESSAGE_FLAG_EPHEMERAL \
    )

#define DCC_MESSAGE_TEXT_EMBED_ACTIONS(content_, embed_, ...) \
    DCC_MESSAGE_TEXT_EMBEDS_ACTIONS_ARRAY( \
        (content_), \
        DCC_SUGAR_ARRAY(dcc_embed_builder_t, (embed_)), \
        1U, \
        DCC_SUGAR_ARRAY(dcc_component_builder_t, __VA_ARGS__), \
        DCC_SUGAR_ARRAY_LEN(dcc_component_builder_t, __VA_ARGS__), \
        0U \
    )

#define DCC_MESSAGE_EPHEMERAL_TEXT_EMBED_ACTIONS(content_, embed_, ...) \
    DCC_MESSAGE_TEXT_EMBEDS_ACTIONS_ARRAY( \
        (content_), \
        DCC_SUGAR_ARRAY(dcc_embed_builder_t, (embed_)), \
        1U, \
        DCC_SUGAR_ARRAY(dcc_component_builder_t, __VA_ARGS__), \
        DCC_SUGAR_ARRAY_LEN(dcc_component_builder_t, __VA_ARGS__), \
        DCC_MESSAGE_FLAG_EPHEMERAL \
    )

#define DCC_REPLY_TEXT_ACTIONS(ctx_, content_, ...) \
    DCC_REPLY((ctx_), DCC_MESSAGE_TEXT_ACTIONS((content_), __VA_ARGS__))
#define DCC_REPLY_EPHEMERAL_TEXT_ACTIONS(ctx_, content_, ...) \
    DCC_REPLY((ctx_), DCC_MESSAGE_EPHEMERAL_TEXT_ACTIONS((content_), __VA_ARGS__))
#define DCC_REPLY_EMBED_ACTIONS(ctx_, embed_, ...) \
    DCC_REPLY((ctx_), DCC_MESSAGE_EMBED_ACTIONS((embed_), __VA_ARGS__))
#define DCC_REPLY_EPHEMERAL_EMBED_ACTIONS(ctx_, embed_, ...) \
    DCC_REPLY((ctx_), DCC_MESSAGE_EPHEMERAL_EMBED_ACTIONS((embed_), __VA_ARGS__))
#define DCC_REPLY_TEXT_EMBED_ACTIONS(ctx_, content_, embed_, ...) \
    DCC_REPLY((ctx_), DCC_MESSAGE_TEXT_EMBED_ACTIONS((content_), (embed_), __VA_ARGS__))
#define DCC_REPLY_EPHEMERAL_TEXT_EMBED_ACTIONS(ctx_, content_, embed_, ...) \
    DCC_REPLY((ctx_), DCC_MESSAGE_EPHEMERAL_TEXT_EMBED_ACTIONS((content_), (embed_), __VA_ARGS__))

#define DCC_FOLLOWUP_EMBED_ACTIONS(ctx_, embed_, ...) \
    DCC_FOLLOWUP((ctx_), DCC_MESSAGE_EMBED_ACTIONS((embed_), __VA_ARGS__))
#define DCC_UPDATE_EMBED_ACTIONS(ctx_, embed_, ...) \
    DCC_UPDATE((ctx_), DCC_MESSAGE_EMBED_ACTIONS((embed_), __VA_ARGS__))
#define DCC_SEND_EMBED_ACTIONS(ctx_, embed_, ...) \
    DCC_SEND((ctx_), DCC_MESSAGE_EMBED_ACTIONS((embed_), __VA_ARGS__))

#define DCC_APP_SEND_EMBED_ACTIONS(app_, channel_id_, embed_, ...) \
    DCC_APP_SEND((app_), (channel_id_), DCC_MESSAGE_EMBED_ACTIONS((embed_), __VA_ARGS__))
#define DCC_APP_EDIT_EMBED_ACTIONS(app_, channel_id_, message_id_, embed_, ...) \
    DCC_APP_EDIT_MESSAGE( \
        (app_), \
        (channel_id_), \
        (message_id_), \
        DCC_MESSAGE_EMBED_ACTIONS((embed_), __VA_ARGS__) \
    )

#endif
