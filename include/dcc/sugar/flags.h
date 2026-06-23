#ifndef DCC_SUGAR_FLAGS_H
#define DCC_SUGAR_FLAGS_H

#include <dcc/message.h>
#include <stdint.h>

#define DCC_SUGAR_FLAGS_ARRAY(type_, ...) ((type_[]){ __VA_ARGS__ })
#define DCC_SUGAR_FLAGS_ARRAY_LEN(type_, ...) (sizeof((type_[]){ __VA_ARGS__ }) / sizeof(type_))

static inline dcc_message_builder_t dcc_sugar_message_with_flags(
    dcc_message_builder_t message,
    uint64_t flags
) {
    message.flags |= flags;
    message.has_flags = 1U;
    return message;
}

#define DCC_MESSAGE_FLAGS(flags_) \
    ((dcc_message_builder_t){ \
        .flags = (flags_), \
        .has_flags = 1U \
    })
#define DCC_MESSAGE_TEXT_FLAGS(content_, flags_) \
    ((dcc_message_builder_t){ \
        .content = (content_), \
        .flags = (flags_), \
        .has_content = 1U, \
        .has_flags = 1U \
    })
#define DCC_MESSAGE_EMBED_FLAGS(embed_, flags_) \
    ((dcc_message_builder_t){ \
        .embeds = DCC_SUGAR_FLAGS_ARRAY(dcc_embed_builder_t, (embed_)), \
        .embeds_count = 1U, \
        .flags = (flags_), \
        .has_flags = 1U \
    })
#define DCC_MESSAGE_TEXT_EMBED_FLAGS(content_, embed_, flags_) \
    ((dcc_message_builder_t){ \
        .content = (content_), \
        .embeds = DCC_SUGAR_FLAGS_ARRAY(dcc_embed_builder_t, (embed_)), \
        .embeds_count = 1U, \
        .flags = (flags_), \
        .has_content = 1U, \
        .has_flags = 1U \
    })

#define DCC_MESSAGE_WITH_FLAGS(message_, flags_) \
    dcc_sugar_message_with_flags((message_), (flags_))
#define DCC_MESSAGE_PRIVATE(message_) \
    DCC_MESSAGE_WITH_FLAGS((message_), DCC_MESSAGE_FLAG_EPHEMERAL)
#define DCC_MESSAGE_SILENT(message_) \
    DCC_MESSAGE_WITH_FLAGS((message_), DCC_MESSAGE_FLAG_SUPPRESS_NOTIFICATIONS)
#define DCC_MESSAGE_NO_EMBEDS(message_) \
    DCC_MESSAGE_WITH_FLAGS((message_), DCC_MESSAGE_FLAG_SUPPRESS_EMBEDS)
#define DCC_MESSAGE_LOADING_STATE(message_) \
    DCC_MESSAGE_WITH_FLAGS((message_), DCC_MESSAGE_FLAG_LOADING)

#define DCC_MESSAGE_TEXT_PRIVATE(content_) \
    DCC_MESSAGE_TEXT_FLAGS((content_), DCC_MESSAGE_FLAG_EPHEMERAL)
#define DCC_MESSAGE_TEXT_SILENT(content_) \
    DCC_MESSAGE_TEXT_FLAGS((content_), DCC_MESSAGE_FLAG_SUPPRESS_NOTIFICATIONS)
#define DCC_MESSAGE_TEXT_NO_EMBEDS(content_) \
    DCC_MESSAGE_TEXT_FLAGS((content_), DCC_MESSAGE_FLAG_SUPPRESS_EMBEDS)

#define DCC_MESSAGE_PRIVATE_EMBED(embed_) \
    DCC_MESSAGE_EMBED_FLAGS((embed_), DCC_MESSAGE_FLAG_EPHEMERAL)
#define DCC_MESSAGE_SILENT_EMBED(embed_) \
    DCC_MESSAGE_EMBED_FLAGS((embed_), DCC_MESSAGE_FLAG_SUPPRESS_NOTIFICATIONS)
#define DCC_MESSAGE_PRIVATE_TEXT_EMBED(content_, embed_) \
    DCC_MESSAGE_TEXT_EMBED_FLAGS((content_), (embed_), DCC_MESSAGE_FLAG_EPHEMERAL)
#define DCC_MESSAGE_SILENT_TEXT_EMBED(content_, embed_) \
    DCC_MESSAGE_TEXT_EMBED_FLAGS((content_), (embed_), DCC_MESSAGE_FLAG_SUPPRESS_NOTIFICATIONS)

#define DCC_MESSAGE_PRIVATE_V2(...) \
    ((dcc_message_builder_t){ \
        .components_v2 = DCC_SUGAR_FLAGS_ARRAY(dcc_component_v2_builder_t, __VA_ARGS__), \
        .components_v2_count = DCC_SUGAR_FLAGS_ARRAY_LEN(dcc_component_v2_builder_t, __VA_ARGS__), \
        .flags = DCC_MESSAGE_FLAG_IS_COMPONENTS_V2 | DCC_MESSAGE_FLAG_EPHEMERAL, \
        .has_flags = 1U \
    })
#define DCC_MESSAGE_SILENT_V2(...) \
    ((dcc_message_builder_t){ \
        .components_v2 = DCC_SUGAR_FLAGS_ARRAY(dcc_component_v2_builder_t, __VA_ARGS__), \
        .components_v2_count = DCC_SUGAR_FLAGS_ARRAY_LEN(dcc_component_v2_builder_t, __VA_ARGS__), \
        .flags = DCC_MESSAGE_FLAG_IS_COMPONENTS_V2 | DCC_MESSAGE_FLAG_SUPPRESS_NOTIFICATIONS, \
        .has_flags = 1U \
    })

#endif
