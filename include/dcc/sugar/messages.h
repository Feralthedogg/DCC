#ifndef DCC_SUGAR_MESSAGES_H
#define DCC_SUGAR_MESSAGES_H

#include <dcc/component.h>
#include <dcc/component_v2.h>
#include <dcc/embed.h>
#include <dcc/message.h>

#ifndef DCC_SUGAR_ARRAY
#define DCC_SUGAR_ARRAY(type_, ...) ((type_[]){ __VA_ARGS__ })
#endif

#ifndef DCC_SUGAR_ARRAY_LEN
#define DCC_SUGAR_ARRAY_LEN(type_, ...) (sizeof((type_[]){ __VA_ARGS__ }) / sizeof(type_))
#endif

#ifndef DCC_MESSAGE_EMPTY
#define DCC_MESSAGE_EMPTY() ((dcc_message_builder_t){0})
#endif

#ifndef DCC_MESSAGE_TEXT
#define DCC_MESSAGE_TEXT(content_) \
    ((dcc_message_builder_t){ \
        .content = (content_), \
        .has_content = 1U \
    })
#endif

#ifndef DCC_MESSAGE_TTS
#define DCC_MESSAGE_TTS(content_) \
    ((dcc_message_builder_t){ \
        .content = (content_), \
        .tts = 1U, \
        .has_content = 1U, \
        .has_tts = 1U \
    })
#endif

#ifndef DCC_MESSAGE_EPHEMERAL
#define DCC_MESSAGE_EPHEMERAL(content_) \
    ((dcc_message_builder_t){ \
        .content = (content_), \
        .flags = DCC_MESSAGE_FLAG_EPHEMERAL, \
        .has_content = 1U, \
        .has_flags = 1U \
    })
#endif

#ifndef DCC_MESSAGE_SUPPRESS_EMBEDS
#define DCC_MESSAGE_SUPPRESS_EMBEDS(content_) \
    ((dcc_message_builder_t){ \
        .content = (content_), \
        .flags = DCC_MESSAGE_FLAG_SUPPRESS_EMBEDS, \
        .has_content = 1U, \
        .has_flags = 1U \
    })
#endif

#ifndef DCC_MESSAGE_EMBEDS_ARRAY
#define DCC_MESSAGE_EMBEDS_ARRAY(embeds_, embed_count_) \
    ((dcc_message_builder_t){ \
        .embeds = (embeds_), \
        .embeds_count = (embed_count_) \
    })
#endif

#ifndef DCC_MESSAGE_EMBEDS
#define DCC_MESSAGE_EMBEDS(...) \
    ((dcc_message_builder_t){ \
        .embeds = DCC_SUGAR_ARRAY(dcc_embed_builder_t, __VA_ARGS__), \
        .embeds_count = DCC_SUGAR_ARRAY_LEN(dcc_embed_builder_t, __VA_ARGS__) \
    })
#endif

#ifndef DCC_MESSAGE_TEXT_EMBEDS_ARRAY
#define DCC_MESSAGE_TEXT_EMBEDS_ARRAY(content_, embeds_, embed_count_) \
    ((dcc_message_builder_t){ \
        .content = (content_), \
        .embeds = (embeds_), \
        .embeds_count = (embed_count_), \
        .has_content = 1U \
    })
#endif

#ifndef DCC_MESSAGE_TEXT_EMBEDS
#define DCC_MESSAGE_TEXT_EMBEDS(content_, ...) \
    ((dcc_message_builder_t){ \
        .content = (content_), \
        .embeds = DCC_SUGAR_ARRAY(dcc_embed_builder_t, __VA_ARGS__), \
        .embeds_count = DCC_SUGAR_ARRAY_LEN(dcc_embed_builder_t, __VA_ARGS__), \
        .has_content = 1U \
    })
#endif

#ifndef DCC_MESSAGE_COMPONENTS_ARRAY
#define DCC_MESSAGE_COMPONENTS_ARRAY(components_, component_count_) \
    ((dcc_message_builder_t){ \
        .components = (components_), \
        .components_count = (component_count_) \
    })
#endif

#ifndef DCC_MESSAGE_COMPONENTS
#define DCC_MESSAGE_COMPONENTS(...) \
    ((dcc_message_builder_t){ \
        .components = DCC_SUGAR_ARRAY(dcc_component_builder_t, __VA_ARGS__), \
        .components_count = DCC_SUGAR_ARRAY_LEN(dcc_component_builder_t, __VA_ARGS__) \
    })
#endif

#ifndef DCC_MESSAGE_COMPONENTS_JSON
#define DCC_MESSAGE_COMPONENTS_JSON(components_json_) \
    ((dcc_message_builder_t){ \
        .components_json = (components_json_) \
    })
#endif

#ifndef DCC_MESSAGE_COMPONENTS_V2_ARRAY
#define DCC_MESSAGE_COMPONENTS_V2_ARRAY(components_, component_count_) \
    ((dcc_message_builder_t){ \
        .components_v2 = (components_), \
        .components_v2_count = (component_count_), \
        .flags = DCC_MESSAGE_FLAG_IS_COMPONENTS_V2, \
        .has_flags = 1U \
    })
#endif

#ifndef DCC_MESSAGE_COMPONENTS_V2
#define DCC_MESSAGE_COMPONENTS_V2(...) \
    ((dcc_message_builder_t){ \
        .components_v2 = DCC_SUGAR_ARRAY(dcc_component_v2_builder_t, __VA_ARGS__), \
        .components_v2_count = DCC_SUGAR_ARRAY_LEN(dcc_component_v2_builder_t, __VA_ARGS__), \
        .flags = DCC_MESSAGE_FLAG_IS_COMPONENTS_V2, \
        .has_flags = 1U \
    })
#endif

#ifndef DCC_MESSAGE_COMPONENTS_V2_FLAGS
#define DCC_MESSAGE_COMPONENTS_V2_FLAGS(flags_, ...) \
    ((dcc_message_builder_t){ \
        .components_v2 = DCC_SUGAR_ARRAY(dcc_component_v2_builder_t, __VA_ARGS__), \
        .components_v2_count = DCC_SUGAR_ARRAY_LEN(dcc_component_v2_builder_t, __VA_ARGS__), \
        .flags = ((flags_) | DCC_MESSAGE_FLAG_IS_COMPONENTS_V2), \
        .has_flags = 1U \
    })
#endif

#ifndef DCC_MESSAGE_EPHEMERAL_COMPONENTS_V2
#define DCC_MESSAGE_EPHEMERAL_COMPONENTS_V2(...) \
    DCC_MESSAGE_COMPONENTS_V2_FLAGS(DCC_MESSAGE_FLAG_EPHEMERAL, __VA_ARGS__)
#endif

#ifndef DCC_MESSAGE_COMPONENTS_V2_JSON
#define DCC_MESSAGE_COMPONENTS_V2_JSON(components_json_) \
    ((dcc_message_builder_t){ \
        .components_v2_json = (components_json_), \
        .flags = DCC_MESSAGE_FLAG_IS_COMPONENTS_V2, \
        .has_flags = 1U \
    })
#endif

#ifndef DCC_MESSAGE_COMPONENTS_V2_JSON_FLAGS
#define DCC_MESSAGE_COMPONENTS_V2_JSON_FLAGS(flags_, components_json_) \
    ((dcc_message_builder_t){ \
        .components_v2_json = (components_json_), \
        .flags = ((flags_) | DCC_MESSAGE_FLAG_IS_COMPONENTS_V2), \
        .has_flags = 1U \
    })
#endif

#ifndef DCC_MESSAGE_EPHEMERAL_COMPONENTS_V2_JSON
#define DCC_MESSAGE_EPHEMERAL_COMPONENTS_V2_JSON(components_json_) \
    DCC_MESSAGE_COMPONENTS_V2_JSON_FLAGS(DCC_MESSAGE_FLAG_EPHEMERAL, (components_json_))
#endif

#endif
