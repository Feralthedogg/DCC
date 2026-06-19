#ifndef DCC_REST_TYPES_MESSAGES_H
#define DCC_REST_TYPES_MESSAGES_H

#include <dcc/rest/base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_message_reaction_params {
    size_t size;
    dcc_snowflake_t channel_id;
    dcc_snowflake_t message_id;
    const char *reaction;
} dcc_message_reaction_params_t;

typedef struct dcc_message_flags_params {
    size_t size;
    dcc_snowflake_t channel_id;
    dcc_snowflake_t message_id;
    uint64_t flags;
} dcc_message_flags_params_t;

typedef enum dcc_message_search_has {
    DCC_MESSAGE_SEARCH_HAS_IMAGE = 0,
    DCC_MESSAGE_SEARCH_HAS_SOUND,
    DCC_MESSAGE_SEARCH_HAS_VIDEO,
    DCC_MESSAGE_SEARCH_HAS_FILE,
    DCC_MESSAGE_SEARCH_HAS_STICKER,
    DCC_MESSAGE_SEARCH_HAS_EMBED,
    DCC_MESSAGE_SEARCH_HAS_LINK,
    DCC_MESSAGE_SEARCH_HAS_POLL,
    DCC_MESSAGE_SEARCH_HAS_SNAPSHOT
} dcc_message_search_has_t;

typedef enum dcc_message_search_sort {
    DCC_MESSAGE_SEARCH_SORT_TIMESTAMP = 0,
    DCC_MESSAGE_SEARCH_SORT_RELEVANCE
} dcc_message_search_sort_t;

typedef enum dcc_message_search_order {
    DCC_MESSAGE_SEARCH_ORDER_DESC = 0,
    DCC_MESSAGE_SEARCH_ORDER_ASC
} dcc_message_search_order_t;

typedef struct dcc_message_search_params {
    size_t size;
    const char *content;
    const dcc_snowflake_t *author_ids;
    size_t author_id_count;
    const dcc_snowflake_t *mentions;
    size_t mention_count;
    const dcc_snowflake_t *channel_ids;
    size_t channel_id_count;
    const dcc_snowflake_t *mention_role_ids;
    size_t mention_role_id_count;
    const dcc_snowflake_t *replied_to_user_ids;
    size_t replied_to_user_id_count;
    const dcc_snowflake_t *replied_to_message_ids;
    size_t replied_to_message_id_count;
    const char * const *author_types;
    size_t author_type_count;
    const dcc_message_search_has_t *has;
    size_t has_count;
    const char * const *embed_types;
    size_t embed_type_count;
    const char * const *embed_providers;
    size_t embed_provider_count;
    const char * const *link_hostnames;
    size_t link_hostname_count;
    const char * const *attachment_filenames;
    size_t attachment_filename_count;
    const char * const *attachment_extensions;
    size_t attachment_extension_count;
    dcc_snowflake_t max_id;
    dcc_snowflake_t min_id;
    uint8_t has_pinned;
    uint8_t pinned;
    uint8_t has_mention_everyone;
    uint8_t mention_everyone;
    uint8_t has_include_nsfw;
    uint8_t include_nsfw;
    uint8_t has_limit;
    uint32_t limit;
    uint8_t has_offset;
    uint32_t offset;
    uint8_t has_slop;
    uint32_t slop;
    uint8_t has_sort_by;
    dcc_message_search_sort_t sort_by;
    uint8_t has_sort_order;
    dcc_message_search_order_t sort_order;
} dcc_message_search_params_t;

#ifdef __cplusplus
}
#endif

#endif
