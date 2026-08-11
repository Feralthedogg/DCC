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

#define DCC_MESSAGE_FLAG_SUPPRESS_EMBEDS UINT64_C(4)
#define DCC_MESSAGE_FLAG_EPHEMERAL UINT64_C(64)
#define DCC_MESSAGE_FLAG_LOADING UINT64_C(128)
#define DCC_MESSAGE_FLAG_SUPPRESS_NOTIFICATIONS UINT64_C(4096)
#define DCC_MESSAGE_FLAG_IS_COMPONENTS_V2 UINT64_C(32768)

#define DCC_POLL_BUILDER_VERSION 1U
#define DCC_ALLOWED_MENTIONS_BUILDER_VERSION 1U
#define DCC_MESSAGE_BUILDER_VERSION 1U

#define DCC_POLL_BUILDER_PRESENT_QUESTION UINT64_C(1)
#define DCC_POLL_BUILDER_PRESENT_ANSWERS (UINT64_C(1) << 1U)
#define DCC_POLL_BUILDER_PRESENT_DURATION_HOURS (UINT64_C(1) << 2U)
#define DCC_POLL_BUILDER_PRESENT_LAYOUT_TYPE (UINT64_C(1) << 3U)
#define DCC_POLL_BUILDER_PRESENT_ALLOW_MULTISELECT (UINT64_C(1) << 4U)

#define DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_USERS UINT64_C(1)
#define DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_ROLES (UINT64_C(1) << 1U)
#define DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_PARSE_USERS (UINT64_C(1) << 2U)
#define DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_PARSE_ROLES (UINT64_C(1) << 3U)
#define DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_PARSE_EVERYONE (UINT64_C(1) << 4U)
#define DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_REPLIED_USER (UINT64_C(1) << 5U)

#define DCC_MESSAGE_BUILDER_PRESENT_CONTENT UINT64_C(1)
#define DCC_MESSAGE_BUILDER_PRESENT_NONCE (UINT64_C(1) << 1U)
#define DCC_MESSAGE_BUILDER_PRESENT_TTS (UINT64_C(1) << 2U)
#define DCC_MESSAGE_BUILDER_PRESENT_FLAGS (UINT64_C(1) << 3U)
#define DCC_MESSAGE_BUILDER_PRESENT_STICKER_IDS (UINT64_C(1) << 4U)
#define DCC_MESSAGE_BUILDER_PRESENT_EMBEDS (UINT64_C(1) << 5U)
#define DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS (UINT64_C(1) << 6U)
#define DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS_V2 (UINT64_C(1) << 7U)
#define DCC_MESSAGE_BUILDER_PRESENT_POLL (UINT64_C(1) << 8U)
#define DCC_MESSAGE_BUILDER_PRESENT_ALLOWED_MENTIONS (UINT64_C(1) << 9U)
/* Transition-only raw JSON groups; removed with the 1.x compatibility surface. */
#define DCC_MESSAGE_BUILDER_PRESENT_MESSAGE_REFERENCE_JSON (UINT64_C(1) << 10U)
#define DCC_MESSAGE_BUILDER_PRESENT_ALLOWED_MENTIONS_JSON (UINT64_C(1) << 11U)
#define DCC_MESSAGE_BUILDER_PRESENT_EMBEDS_JSON (UINT64_C(1) << 12U)
#define DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS_JSON (UINT64_C(1) << 13U)
#define DCC_MESSAGE_BUILDER_PRESENT_ATTACHMENTS_JSON (UINT64_C(1) << 14U)
#define DCC_MESSAGE_BUILDER_PRESENT_POLL_JSON (UINT64_C(1) << 15U)
#define DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS_V2_JSON (UINT64_C(1) << 16U)

#ifdef __cplusplus
extern "C" {
#endif

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
    uint32_t version;
    uint64_t present;
    dcc_poll_media_t question;
    const dcc_poll_answer_t *answers;
    size_t answer_count;
    uint32_t duration_hours;
    dcc_poll_layout_type_t layout_type;
    uint8_t allow_multiselect;
} dcc_poll_builder_t;

typedef struct dcc_allowed_mentions_builder {
    size_t size;
    uint32_t version;
    uint64_t present;
    const dcc_snowflake_t *users;
    size_t user_count;
    const dcc_snowflake_t *roles;
    size_t role_count;
    uint8_t parse_users;
    uint8_t parse_roles;
    uint8_t parse_everyone;
    uint8_t replied_user;
} dcc_allowed_mentions_builder_t;

typedef struct dcc_message_builder {
    size_t size;
    uint32_t version;
    uint64_t present;
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
    const dcc_allowed_mentions_builder_t *allowed_mentions;
    const dcc_snowflake_t *sticker_ids;
    size_t sticker_ids_count;
    uint64_t flags;
    uint8_t tts;
} dcc_message_builder_t;

#define DCC_POLL_BUILDER_INIT \
    { \
        sizeof(dcc_poll_builder_t), DCC_POLL_BUILDER_VERSION, UINT64_C(0), \
        { NULL, { NULL, 0U, 0U, 0U, 0U }, 0U }, NULL, 0U, 0U, \
        (dcc_poll_layout_type_t)0, 0U \
    }
#define DCC_ALLOWED_MENTIONS_BUILDER_INIT \
    { \
        sizeof(dcc_allowed_mentions_builder_t), \
        DCC_ALLOWED_MENTIONS_BUILDER_VERSION, UINT64_C(0), \
        NULL, 0U, NULL, 0U, 0U, 0U, 0U, 0U \
    }
#define DCC_MESSAGE_BUILDER_INIT \
    { \
        sizeof(dcc_message_builder_t), DCC_MESSAGE_BUILDER_VERSION, UINT64_C(0), \
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, \
        NULL, 0U, NULL, 0U, NULL, 0U, NULL, NULL, NULL, NULL, 0U, 0U, 0U \
    }

DCC_API void dcc_message_builder_init(dcc_message_builder_t *builder);
DCC_API void dcc_poll_builder_init(dcc_poll_builder_t *builder);
DCC_API void dcc_allowed_mentions_builder_init(dcc_allowed_mentions_builder_t *builder);
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
DCC_API dcc_status_t dcc_message_builder_set_allowed_mentions(
    dcc_message_builder_t *builder,
    const dcc_allowed_mentions_builder_t *allowed_mentions
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
