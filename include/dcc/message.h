#ifndef DCC_MESSAGE_H
#define DCC_MESSAGE_H

#include <dcc/component.h>
#include <dcc/component_v2.h>
#include <dcc/embed.h>
#include <dcc/error.h>
#include <dcc/export.h>
#include <dcc/snowflake.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DCC_MESSAGE_FLAG_IS_COMPONENTS_V2 (1ULL << 15)

typedef enum dcc_poll_layout_type {
    DCC_POLL_LAYOUT_DEFAULT = 1
} dcc_poll_layout_type_t;

typedef struct dcc_poll_media {
    const char *text;
    dcc_component_emoji_t emoji;
    uint8_t has_emoji;
} dcc_poll_media_t;

typedef struct dcc_poll_answer {
    dcc_poll_media_t media;
} dcc_poll_answer_t;

typedef struct dcc_poll_builder {
    size_t size;
    dcc_poll_media_t question;
    const dcc_poll_answer_t *answers;
    size_t answer_count;
    uint32_t duration_hours;
    dcc_poll_layout_type_t layout_type;
    uint8_t allow_multiselect;
    uint8_t has_duration;
    uint8_t has_layout_type;
    uint8_t has_allow_multiselect;
} dcc_poll_builder_t;

typedef struct dcc_message_builder {
    const char *content;
    const char *nonce;
    const char *message_reference_json;
    const char *allowed_mentions_json;
    const char *embeds_json;
    const char *components_json;
    const char *attachments_json;
    const char *poll_json;
    const dcc_embed_builder_t *embeds;
    size_t embeds_count;
    const dcc_component_builder_t *components;
    size_t components_count;
    const dcc_component_v2_builder_t *components_v2;
    size_t components_v2_count;
    const char *components_v2_json;
    const dcc_poll_builder_t *poll;
    const dcc_snowflake_t *sticker_ids;
    size_t sticker_ids_count;
    uint64_t flags;
    uint8_t tts;
    uint8_t has_content;
    uint8_t has_nonce;
    uint8_t has_tts;
    uint8_t has_flags;
} dcc_message_builder_t;

DCC_API void dcc_message_builder_init(dcc_message_builder_t *builder);
DCC_API dcc_status_t dcc_message_builder_set_content(dcc_message_builder_t *builder, const char *content);
DCC_API dcc_status_t dcc_message_builder_set_nonce(dcc_message_builder_t *builder, const char *nonce);
DCC_API dcc_status_t dcc_message_builder_set_tts(dcc_message_builder_t *builder, uint8_t tts);
DCC_API dcc_status_t dcc_message_builder_set_flags(dcc_message_builder_t *builder, uint64_t flags);
DCC_API dcc_status_t dcc_message_builder_set_sticker_ids(
    dcc_message_builder_t *builder,
    const dcc_snowflake_t *sticker_ids,
    size_t sticker_ids_count
);
DCC_API dcc_status_t dcc_message_builder_set_message_reference_json(
    dcc_message_builder_t *builder,
    const char *message_reference_json
);
DCC_API dcc_status_t dcc_message_builder_set_allowed_mentions_json(
    dcc_message_builder_t *builder,
    const char *allowed_mentions_json
);
DCC_API dcc_status_t dcc_message_builder_set_embeds(
    dcc_message_builder_t *builder,
    const dcc_embed_builder_t *embeds,
    size_t embeds_count
);
DCC_API dcc_status_t dcc_message_builder_set_components(
    dcc_message_builder_t *builder,
    const dcc_component_builder_t *components,
    size_t components_count
);
DCC_API dcc_status_t dcc_message_builder_set_components_v2(
    dcc_message_builder_t *builder,
    const dcc_component_v2_builder_t *components,
    size_t components_count
);
DCC_API dcc_status_t dcc_message_builder_set_embeds_json(dcc_message_builder_t *builder, const char *embeds_json);
DCC_API dcc_status_t dcc_message_builder_set_components_json(
    dcc_message_builder_t *builder,
    const char *components_json
);
DCC_API dcc_status_t dcc_message_builder_set_components_v2_json(
    dcc_message_builder_t *builder,
    const char *components_json
);
DCC_API dcc_status_t dcc_message_builder_set_attachments_json(
    dcc_message_builder_t *builder,
    const char *attachments_json
);
DCC_API dcc_status_t dcc_message_builder_set_poll(
    dcc_message_builder_t *builder,
    const dcc_poll_builder_t *poll
);
DCC_API dcc_status_t dcc_message_builder_set_poll_json(dcc_message_builder_t *builder, const char *poll_json);
DCC_API dcc_status_t dcc_message_poll_builder_build_json(const dcc_poll_builder_t *poll, char **out_json);
DCC_API void dcc_message_poll_builder_json_free(char *json);
DCC_API dcc_status_t dcc_message_builder_build_json(const dcc_message_builder_t *builder, char **out_json);
DCC_API void dcc_message_builder_json_free(char *json);

#ifdef __cplusplus
}
#endif

#endif
